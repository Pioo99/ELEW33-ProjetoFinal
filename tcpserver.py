import socket
from PIL import Image
import io
import numpy as np
from time import sleep

TCP_IP = '192.168.18.4'
TCP_PORT = 5000

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)

print("Esperando conexão...")

conn, addr = s.accept()
print("IP conectado:", addr)

def send_image(image_path):
    try:
        imagem = Image.open(image_path)
        buffer = io.BytesIO()
        imagem.save(buffer, format='JPEG')  # Salva a imagem no buffer em formato JPEG
        dados_imagem = buffer.getvalue()  # Obtém bytes da imagem

        # Envia o tamanho da imagem primeiro
        conn.sendall(len(dados_imagem).to_bytes(4, 'big'))
        
        # Envia a imagem
        conn.sendall(dados_imagem)

        print(f"Imagem '{image_path}' enviada com sucesso!")

    except Exception as e:
        print(f"Erro ao enviar imagem: {e}")

def receive_histogram():
    try:
        # Recebe o tamanho do histograma
        hist_size_data = conn.recv(4)
        if not hist_size_data:
            print("Erro ao receber tamanho do histograma")
            return None
        
        hist_size = int.from_bytes(hist_size_data, 'big')
        
        # Recebe o histograma (em bytes) de acordo com o tamanho recebido
        histogram_data = conn.recv(hist_size)
        if not histogram_data:
            print("Erro ao receber histograma")
            return None
        
        # Converte os bytes recebidos em um array de números inteiros (histograma)
        histogram = np.frombuffer(histogram_data, dtype=np.uint32)
        
        print("Histograma recebido:", histogram)
        return histogram
    except Exception as e:
        print(f"Erro ao receber histograma: {e}")
        return None

while True:
    try:
        caminho_imagem = "imagens/minha_imagem.jpg"  # Ajuste o caminho
        send_image(caminho_imagem)
        
        # Recebe e imprime o histograma
        histograma = receive_histogram()
        if histograma is not None:
            print("Histograma recebido:", histograma)
        
        sleep(5)  # Espera entre os ciclos de envio e recepção

    except KeyboardInterrupt:
        break

conn.close()