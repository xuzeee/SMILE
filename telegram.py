from flask import Flask, request
import requests
import os
import telebot
from telebot import types
import threading

# Token do bot
BOT_TOKEN = '7543094329:AAFoq_B8oq_eEq4BijCS4CXvbOgLFOj2NBo'
bot = telebot.TeleBot(BOT_TOKEN)
app = Flask(__name__)

# Credenciais do dispositivo Thinger.io
THINGER_USERNAME = 'picoantonio'
THINGER_DEVICE_ID = 'picobot'
THINGER_DEVICE_CREDENTIAL = 'Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiJEZXZpY2VDYWxsYmFja19waWNvYm90Iiwic3ZyIjoidXMtZWFzdC5hd3MudGhpbmdlci5pbyIsInVzciI6InBpY29hbnRvbmlvIn0.VFPZNCvCHGFax2mErQEoYMZUYvEGr2xdCw8gWhGi0lc'

# ID do chat do Telegram
TELEGRAM_CHAT_ID = '1100548115'

# ✅ Variável global para armazenar o valor de kWh
current_kwh = None
total = None
estimate = None

@app.route('/update', methods=['GET'])
def update():
    global current_kwh
    global total
    global estimate
    print("Received a request")
    print(f"Request args: {request.args}")

    # Obtém os parâmetros enviados na URL
    kwh = request.args.get('kw')
    estimate = request.args.get('estimate')
    total = request.args.get('total')

    if kwh and estimate and total:
        print(f"Received Potency: {kwh} W")
        current_kwh = float(kwh)  # ✅ Armazena o valor em uma variável global
        estimate = float(estimate)
        total = float(total)
        # Envia os dados para o Thinger.io
        url = 'https://backend.thinger.io/v3/users/picoantonio/devices/picobot/callback/data'
        headers = {
            'Content-Type': 'application/json',
            'Authorization': THINGER_DEVICE_CREDENTIAL
        }
        data = {
            'kwh': current_kwh,
            'estimate': estimate,
            'total': total
        }

        response = requests.post(url, json=data, headers=headers)
        if response.status_code == 200:
            return f"Potência {kwh} W recebida e armazenada no Thinger.io", 200
        else:
            return f"Falha ao enviar dados para o Thinger.io: {response.text}", response.status_code
    else:
        print("No kW data received")
        return "No kW data received", 400

# ✅ Função para criar o menu do bot
def menu_principal():
    markup = types.InlineKeyboardMarkup()
    botao1 = types.InlineKeyboardButton("⚡ Consumo Atual", callback_data="opcao1")
    botao4 = types.InlineKeyboardButton("⚡ Consumo Total", callback_data="total")
    botao5 = types.InlineKeyboardButton("💲 Estimativa em reais", callback_data="reais")
    botao2 = types.InlineKeyboardButton("📊 Gráfico", url="https://console.thinger.io/dashboards/picoluz?authorization=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiJEYXNoYm9hcmRfcGljb2x1eiIsInN2ciI6InVzLWVhc3QuYXdzLnRoaW5nZXIuaW8iLCJ1c3IiOiJwaWNvYW50b25pbyJ9.Gz7Jsj0EJG6S-enDk0_zaW8ETvWbt-Oau6khVkYo2y8")
    botao3 = types.InlineKeyboardButton("❌ Sair", callback_data="sair")
    
    markup.add(botao1, botao2, botao4, botao5)
    markup.add(botao3)
    return markup

# ✅ Comando /start para exibir o menu
@bot.message_handler(commands=['start'])
def send_welcome(message):
    bot.send_message(
        chat_id=message.chat.id,
        text="Escolha uma opção:",
        reply_markup=menu_principal()
    )

# ✅ Handler para os botões
@bot.callback_query_handler(func=lambda call: True)
def callback_handler(call):
    global current_kwh
    global total
    global estimate
    
    if call.data == "opcao1":
        if current_kwh is not None:
            bot.send_message(chat_id=call.message.chat.id, text=f"⚡ Consumo Atual: {current_kwh} W")
        else:
            bot.send_message(chat_id=call.message.chat.id, text="❌ Dados de consumo não disponíveis.")
    if call.data == "total":
        if total is not None:
            bot.send_message(chat_id=call.message.chat.id, text=f"⚡ Consumo Atual: {total} kWh")
        else:
            bot.send_message(chat_id=call.message.chat.id, text="❌ Dados de consumo não disponíveis.")
    if call.data == "reais":
        if estimate is not None:
            bot.send_message(chat_id=call.message.chat.id, text=f"⚡ Estimativa para o Fim do mês: R$ {estimate} Reais")
        else:
            bot.send_message(chat_id=call.message.chat.id, text="❌ Dados não disponíveis.") 
    if call.data == "sair":
        bot.send_message(chat_id=call.message.chat.id, text="👋 Até mais!")

# ✅ Função para rodar o Flask
def run_flask():
    app.run(host='0.0.0.0', port=5000)

# ✅ Função para rodar o bot
def run_telegram_bot():
    bot.infinity_polling()

# ✅ Inicia Flask e o bot em threads separadas
if __name__ == '__main__':
    flask_thread = threading.Thread(target=run_flask)
    telegram_thread = threading.Thread(target=run_telegram_bot)

    flask_thread.start()
    telegram_thread.start()

    flask_thread.join()
    telegram_thread.join()
