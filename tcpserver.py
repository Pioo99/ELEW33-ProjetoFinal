import socket
from random import randint
from time import sleep

def read_sensor():
    return str(randint(0, 255))

TCP_IP = '192.168.0.221'
TCP_PORT = 5000

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)
print("Esperando conexao")

conn, addr = s.accept()
print ('IP conectado:', addr)
while 1:
    try:
        sensor = read_sensor()
        val = sensor.encode()
        print(f"Enviando valor: {sensor}")
        conn.send(val)
        sleep(2)
    except KeyboardInterrupt:
        break
conn.close()