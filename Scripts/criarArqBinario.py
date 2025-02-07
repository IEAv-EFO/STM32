import struct

# Lista de números em ponto flutuante
numbers = [3.14, 1.618, 2.718, 42.0]

# Criação de um arquivo binário e escrita dos números em ponto flutuante
with open('float_numbers.bin', 'wb') as f:
    for num in numbers:
        f.write(struct.pack('f', num))  # 'f' indica float de 32 bits (4 bytes)
