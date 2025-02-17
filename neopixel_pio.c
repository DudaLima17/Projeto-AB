#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"

// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2818b.pio.h"

// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN 7

// Definição de botões
#define BUTTON_A_PIN 5     // Botão A no GPIO 5
#define BUTTON_B_PIN 6     // Botão B no GPIO 6
#define DEBOUNCE_TIME_MS 200  // Tempo para evitar bouncing nos botões

// Definição de pixel GRB
struct pixel_t {
  uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[LED_COUNT];

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;

/**
 * Inicializa a máquina PIO para controle da matriz de LEDs.
 */
void npInit(uint pin) {
  // Cria programa PIO.
  uint offset = pio_add_program(pio0, &ws2818b_program);
  np_pio = pio0;

  // Toma posse de uma máquina PIO.
  sm = pio_claim_unused_sm(np_pio, false);
  if (sm < 0) {
    np_pio = pio1;
    sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
  }

  // Inicia programa na máquina PIO obtida.
  ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

  // Limpa buffer de pixels.
  for (uint i = 0; i < LED_COUNT; ++i) {
    leds[i].R = 0;
    leds[i].G = 0;
    leds[i].B = 0;
  }
}

/**
 * Atribui uma cor RGB a um LED.
 */
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
  leds[index].R = r;
  leds[index].G = g;
  leds[index].B = b;
}

/**
 * Limpa o buffer de pixels.
 */
void npClear() {
  for (uint i = 0; i < LED_COUNT; ++i)
    npSetLED(i, 0, 0, 0);
}

/**
 * Escreve os dados do buffer nos LEDs.
 */
void npWrite() {
  // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
  for (uint i = 0; i < LED_COUNT; ++i) {
    pio_sm_put_blocking(np_pio, sm, leds[i].G);
    pio_sm_put_blocking(np_pio, sm, leds[i].R);
    pio_sm_put_blocking(np_pio, sm, leds[i].B);
  }
  sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}

// Função para converter a posição da matriz para uma posição do vetor.
int getIndex(int x, int y) {
  // Se a linha for par (0, 2, 4), percorremos da esquerda para a direita.
  // Se a linha for ímpar (1, 3), percorremos da direita para a esquerda.
  if (y % 2 == 0) {
      return y * 5 + x; // Linha par (esquerda para direita).
  } else {
      return y * 5 + (4 - x); // Linha ímpar (direita para esquerda).
  }
}

// Função para exibir a letra "A"
void displayLetterA() {
    int matriz[5][5][3] = {
        {{0, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {0, 0, 0}},
        {{255, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {255, 0, 0}},
        {{255, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {255, 0, 0}},
        {{255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}, {255, 0, 0}},
        {{255, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {255, 0, 0}}
    };

    for (int linha = 0; linha < 5; linha++) {
        for (int coluna = 0; coluna < 5; coluna++) {
            int posicao = getIndex(linha, coluna);
            npSetLED(posicao, matriz[linha][coluna][0], matriz[linha][coluna][1], matriz[linha][coluna][2]);
        }
    }
    npWrite();
}

// Função para exibir a letra "B"
void displayLetterB() {
    int matriz[5][5][3] = {
        {{0, 85, 255}, {0, 85, 255}, {0, 85, 255}, {0, 85, 255}, {0, 0, 0}},
        {{0, 85, 255}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 85, 255}},
        {{0, 85, 255}, {0, 85, 255}, {0, 85, 255}, {0, 85, 255}, {0, 0, 0}},
        {{0, 85, 255}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 85, 255}},
        {{0, 85, 255}, {0, 85, 255}, {0, 85, 255}, {0, 85, 255}, {0, 0, 0}}
    };

    for (int linha = 0; linha < 5; linha++) {
        for (int coluna = 0; coluna < 5; coluna++) {
            int posicao = getIndex(linha, coluna);
            npSetLED(posicao, matriz[linha][coluna][0], matriz[linha][coluna][1], matriz[linha][coluna][2]);
        }
    }
    npWrite();
}

int main() {
  // Configuração dos pinos dos botões como entrada com pull-up
  gpio_init(BUTTON_A_PIN);
  gpio_init(BUTTON_B_PIN);
  gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
  gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_A_PIN);
  gpio_pull_up(BUTTON_B_PIN);

  // Inicializa entradas e saídas.
  stdio_init_all();

  // Inicializa matriz de LEDs NeoPixel.
  npInit(LED_PIN);
  npClear();

  // Loop principal.
  while (true) {
    // Verifica se o botão A foi pressionado.
    if (gpio_get(BUTTON_A_PIN) == 0) {
      displayLetterA(); // Exibe a letra "A".
      sleep_ms(DEBOUNCE_TIME_MS); // Debounce do botão.
    }

    // Verifica se o botão B foi pressionado.
    if (gpio_get(BUTTON_B_PIN) == 0) {
      displayLetterB(); // Exibe a letra "B".
      sleep_ms(DEBOUNCE_TIME_MS); // Debounce do botão.
    }

    // Limpa a matriz após exibir as letras.
    npClear();
    npWrite();
    sleep_ms(100); // Pequeno atraso para evitar flickering.
  }

  return 0;
}