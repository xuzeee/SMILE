# PROJETO SMILE (Sistema de Monitoramento Inteligente de Energia)
Projeto SMILE, desenvolvido para o projeto final do Embarcatech, tem como objetivo monitorar o consumo de energia e notificar o usuário acerca dos gastos.

[Documentação completa](https://docs.google.com/document/d/14P5eBFIyrmmrnY1_6-GfGyncNv3MCAPiVTib1K-lYRI/edit?tab=t.0)

## Sumário
- [Descrição do Funcionamento](#descrição-do-funcionamento)
  - [Interface](#interface)
- [Especificação do Hardware](#especificação-do-hardware)
  - [Componentes Utilizados](#componentes)
  - [Pinagem Utilizada](#pinagem-utilizada)
- [Especificação do Firmware](#especificação-do-firmware)
- [Execução do Projeto](#execução-do-projeto)

## Descrição do funcionamento
O SMILE é um dispositivo criado na placa BitDogLab, usando um sensor de corrente o SCT-013 que ler a corrente da rede e por meio do codigo faz as conversões em corrente e potência, para depois realizar o envio dos dados ao computador de borda e para o display. Havendo duas opções "Consumo Atual" e "Estimativa Mensal", mostrando informações variadas como previsão de gastos e consumo em tempo real.
### Interface
- **Consumo Atual:**
Realiza as leituras em tempo real da potencia em KW, consumo em KWH e estimativa em reais.
- **Estimativa Mensal**
Por meio de um calculo de média do consumo, nessa opção é feita a estimativa de gasto no mês, tanto em KWH como em Reais.
## Espicificação do Hardware
### Componentes
- **Raspberry Pi Pico W**
Microcontrolador com um processador Dual-Core ARM Cortex M0+, rodando em até 133MHz. Possui 264kB de memória SRAM e 2MB de memória Flash integrada na placa. Além de conexões GPIO para conexão de sensores ou atuadores, gerenciamento de interrupções, conectividade WI-FI e Bluetooth, suporte a UART, SPI, I2C, ADC e PIO.
- **Sensor SCT-013**
O Sensor de Corrente Não Invasivo SCT-013-000 é um equipamento para medir corrente até 100A e que não é invasivo. Sem necessidade de cortes de fios para remendos, apenas colocando o fio entre a sua “garra”. 
- **Display OLED SSD1306**
O Display OLED 128x32 0.91 é um display pequeno mas com grande nitidez e baixo consumo de energia. Esse display tem luz própria, não sendo necessária a utilização de Backlight.
- **Botão**
O Push button (botão de pressão) é uma chave que, quando pressionado o botão, ela abre ou fecha o circuito, convertendo assim, um comando mecânico em elétrico. Geralmente eles tem um contato de ação momentânea, abrindo ou fechando o circuito apenas de modo momentâneo.
### Pinagem dos componentes
| Componente           | Pino no Raspberry Pi Pico W (BitDogLab) | Função                                 |
|----------------------|-----------------------------------------|----------------------------------------|
| Botão A              | GPIO 5                                  | Navegar entre os Menus                 |
| Botão B              | GPIO 6                                  | Navegar entre os Menus                 |
| Sensor SCT-013       | GPIO 28                                 | Indicação de estado seguro             |
| Display OLED (SDA)   | GPIO 14                                 | Comunicação I2C – Dados                |
| Display OLED (SCL)   | GPIO 15                                 | Comunicação I2C – Clock                |
## Especificações do Firmware

O firmware foi desenvolvido em linguagem C, utilizando o SDK oficial do Raspberry Pi Pico W e a biblioteca pico-ssd1306 para controle do display OLED. Suas principais funcionalidades incluem:

**Inicialização dos Periféricos**

Logo após a energização do sistema, o firmware executa a inicialização dos seguintes componentes:

- Botões de navegação: Configurados nos pinos GPIO 5 e 6.

- Barramento I2C: Configurado nos pinos SDA (14) e SCL (15) para comunicação com o display OLED.

- Display OLED SSD1306: Inicialização e exibição de telas iniciais.

- ADC: Configuração do ADC para leitura do sensor SCT-013 no pino 28.

**Leitura e Processamento dos Dados**

O firmware realiza leituras periódicas do sensor SCT-013 para medição da corrente elétrica. A corrente RMS é calculada com base em 500 amostras usando a equação:

- Corrente RMS:
  
sqrt(sum / samples)

sum: Soma dos quadrados das amostras de corrente ajustadas.

samples: Número total de amostras coletadas.

sqrt(): Função de raiz quadrada.


Este cálculo segue a definição de RMS (Root Mean Square), que é a raiz quadrada da média dos quadrados dos valores.

- A potência instantânea é obtida por:

potencia = corrente * VOLTAGE_RMS

corrente: Corrente RMS medida.

VOLTAGE_RMS: Tensão RMS da rede elétrica.

potencia: Potência instantânea em watts.


**Interface de Usuário**

A interface do firmware utiliza menus exibidos no display OLED e navegáveis pelos botões A e B:

- Botão A: Alterna entre os modos de medição.
- Botão B: Exibe logs e estatísticas do consumo de energia.
  
Os menus disponíveis incluem:

- Menu Principal: Apresenta as opções de medição.
- Modo Sensores: Exibe a corrente, potência e consumo acumulado.
- Modo Estimativa: Mostra a média de consumo mensal e o custo aproximado.

