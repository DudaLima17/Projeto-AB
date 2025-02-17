
## Projeto de clicar A e B
Funcionalidade: 
Quando apertar o botão A, aparecer A na matriz de leds

Quando apertar o botão B, aparecer B na matriz de leds
```ruby
# Defina a versão mínima do CMake
cmake_minimum_required(VERSION 3.13)

# Inclui os arquivos do SDK da Raspberry Pi Pico
include(pico_sdk_import.cmake)

# Inicializa o SDK do Raspberry Pi Pico
pico_sdk_init()

#Generate PIO header
pico_generate_pio_header(neopixel_pio ${CMAKE_CURRENT_LIST_DIR}/ws2818b.pio)

# Vincula as bibliotecas necessárias para o funcionamento do programa
target_link_libraries(neopixel_pio
    pico_stdlib 
    hardware_pio
    hardware_clocks
)

```
O lado virado para o botão. 


```c
npSetLED(0, 255, 0, 0); // Define o LED de índice 0 para vermelho.
npSetLED(12, 0, 255, 0); // Define o LED de índice 12 (centro da matriz) para verde.
```

