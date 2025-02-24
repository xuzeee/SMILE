# PROJETO SMILE (Sistema de Monitoramento Inteligente de Energia)
Projeto SMILE, desenvolvido para o projeto final do Embarcatech, tem como objetivo monitorar o consumo de energia e notificar o usuário acerca dos gastos.

[Documentação completa](https://docs.google.com/document/d/14P5eBFIyrmmrnY1_6-GfGyncNv3MCAPiVTib1K-lYRI/edit?tab=t.0)

## Sumário
- [Descrição do Funcionamento](#descrição-do-funcionamento)
  - [Interface](#interface)
- [Especificação do Hardware](#especificação-do-hardware)
  - [Componentes Utilizados](#componentes-utilizados)
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
