#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h" // Biblioteca SSD1306 para Pico SDK
#include "hardware/adc.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"

#define SERVER_IP "192.168.137.216"  // IP do computador de borda
#define SERVER_PORT 5000
#define WIFI_SSID "Familia Brandao "
#define WIFI_PASSWORD "994706949"
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C // Endereço I2C do display
#define I2C_SDA 14          // Pino SDA
#define I2C_SCL 15          // Pino SCL
#define BTN_A_PIN 5
#define BTN_B_PIN 6
#define ANALOGICOY 26
#define ANALOGICOX 27

// Instância do display SSD1306
ssd1306_t display;

void demotxt(const char *letra){
    ssd1306_clear(&display);
    ssd1306_draw_string(&display, 1, 35, 1, letra);
    ssd1306_show(&display);
    sleep_ms(1000);
}

void send_to_server(const char *valores){
    struct tcp_pcb *pcb = tcp_new();
    if(!pcb){
        printf("Erro ao criar o PCB\n");
        return;
    }

    ip_addr_t server_ip;
    IP4_ADDR(&server_ip, 192,168,137,216);

    if (tcp_connect(pcb, &server_ip, 5000, NULL) != ERR_OK){
        printf("Erro no servidor\n");
    }

    //cria um display
    char request[256];
    snprintf(request, sizeof(request), "GET /update?temp=%2.f HTTP/1,1\nHost: 10.30.44.103\r\n\r\n", valores);
    tcp_write(pcb, request, strlen(request), TCP_WRITE_FLAG_COPY);
    tcp_close(pcb);

    printf("Conectado com o servidor");

}

typedef struct {
    char text[50];
} OptionText;

typedef struct {
    char header[50];
    OptionText options[3];
    int qntOptions;
} Menu;

void trocaMenu(Menu *newMenu);
Menu *actualMenu;
Menu mainMenu;
Menu op1;
Menu sensorMenu;

void trocaMenu(Menu *newMenu) {
    actualMenu = newMenu;
}

int main() {
    // Inicializa UART para depuração
    stdio_init_all();
    
    // Inicializa botões
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);
    
    gpio_init(BTN_B_PIN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);
    gpio_pull_up(BTN_B_PIN);

    // Inicializa I2C no canal 1
    i2c_init(i2c1, 400 * 1000); // 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display
    if (!ssd1306_init(&display, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_ADDRESS, i2c1)) { 
        printf("Falha ao inicializar o display SSD1306\n");
        return 1;
    }

    // Exibe mensagem inicial
    ssd1306_clear(&display);
    ssd1306_draw_string(&display, 20, 20, 1, "Ligado!");
    ssd1306_show(&display);
    sleep_ms(1000);

    if(cyw43_arch_init()){
        demotxt("Erro ao inicializar o Wi-Fi\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();
    demotxt("Conectando...");
    int result = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000);
    if(result){
        demotxt("Falha ao conectar !");
        return 1;
    }else {
        demotxt("Conectado.\n");
        // Read the ip address in a human readable way
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        printf("Endereço IP %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }
    demotxt("Wifi conectado !");

    // Iniciar ADC e setar joystick
    adc_init();
    adc_gpio_init(ANALOGICOY);
    adc_gpio_init(ANALOGICOX);

    // Inicializar menus
    mainMenu = (Menu) {
        "SMILE MENU",
        {{"Consumo atual"}, {"Consumo Mensal"}, {"Envio de grafico"}},
        3
    };

    op1 = (Menu) {
        "Op1",
        {{"OLA"}, {"MEU"}, {"FRIEND"}},
        3
    };

    sensorMenu = (Menu) {
        "Sensores",
        {{"Valor Y"}, {"Valor X"}, {"Voltar"}},
        3
    };

    actualMenu = &mainMenu;
    int selectedOption = 0;

    while (true) {
        // Leitura dos valores do joystick
        adc_select_input(0);
        uint adc_Y_raw = adc_read();
        adc_select_input(1);
        uint adc_x_raw = adc_read();

        sleep_ms(50);
        ssd1306_clear(&display);

        // Atualizar tela com base no menu atual
        if (actualMenu == &sensorMenu) {
            char sensorY_str[20], sensorX_str[20];
            sprintf(sensorY_str, "Y: %d", adc_Y_raw);
            sprintf(sensorX_str, "X: %d", adc_x_raw);

            ssd1306_draw_string(&display, 10, 10, 1, "Consumo de KWh:");
            ssd1306_draw_string(&display, 20, 30, 1, sensorY_str);
            ssd1306_draw_string(&display, 20, 40, 1, sensorX_str);
            ssd1306_draw_string(&display, 8, 55, 1, "Pressione B p/ voltar");
            ssd1306_show(&display);

            // Se o botão B for pressionado, volta ao menu principal
            if (gpio_get(BTN_B_PIN) == 0) {
                sleep_ms(100);
                trocaMenu(&mainMenu);
            }

            sleep_ms(100); // Pequeno delay para atualizar os valores
            continue; // Pular a lógica do menu normal e voltar ao loop
        }

        // Desenha o cabeçalho do menu
        ssd1306_draw_string(&display, 34, 0, 1, actualMenu->header);

        // Desenhar opções
        for (int i = 0; i < actualMenu->qntOptions; i++) {
            int _x = (selectedOption == i) ? 8 : 0;
            int _y = 20 + i * 12;
            ssd1306_draw_string(&display, _x, _y, 1, actualMenu->options[i].text);
        }

        ssd1306_show(&display);

        // Movimentação no menu com o joystick
        if (adc_Y_raw < 1500) {
            sleep_ms(200);
            selectedOption = (selectedOption + 1) % actualMenu->qntOptions;
        }
        if (adc_Y_raw > 3000) {
            sleep_ms(200);
            selectedOption = (selectedOption - 1 + actualMenu->qntOptions) % actualMenu->qntOptions;
        }

        // Entrar no menu de sensores ao pressionar o botão A
        if (actualMenu == &mainMenu && selectedOption == 0 && gpio_get(BTN_A_PIN) == 0) {
            trocaMenu(&sensorMenu);
        }

        // Trocar para Op1 se selecionado no menu principal
        if (actualMenu == &mainMenu && selectedOption == 1 && gpio_get(BTN_A_PIN) == 0) {
            trocaMenu(&op1);
        }

        // Voltar ao menu principal se pressionar B em qualquer menu diferente do principal
        if (actualMenu != &mainMenu && gpio_get(BTN_B_PIN) == 0) {
            sleep_ms(200);
            trocaMenu(&mainMenu);
        }
    }

    return 0;
}
