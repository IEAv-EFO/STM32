import struct

# Função para converter um número float para sua representação binária
def float_to_binary(value):
    # Empacotando o float como 4 bytes
    packed = struct.pack('>f', value)
    # Convertendo os 4 bytes em um inteiro de 32 bits
    [d] = struct.unpack('>I', packed)
    # Convertendo o inteiro para uma string binária de 32 bits
    return f'{d:032b}'

# Caminho do arquivo binário que você deseja ler
file_path = 'float_numbers.bin'

# Abre o arquivo em modo binário para leitura
with open(file_path, 'rb') as f:
    # Lê os dados do arquivo
    data = f.read()

# Tamanho do dado em ponto flutuante (float de 32 bits = 4 bytes)
float_size = 4

# Itera sobre os dados lidos e converte para ponto flutuante
for i in range(0, len(data), float_size):
    float_data = data[i:i + float_size]
    if len(float_data) == float_size:
        # Converte os bytes lidos para um número em ponto flutuante
        num = struct.unpack('f', float_data)[0]
        # Exibe o número em ponto flutuante
        print(f"Número em ponto flutuante: {num}")

        # Exibe a representação hexadecimal dos dados lidos (ordem correta dos bytes)
        hex_representation = ''.join(f'{byte:02x}' for byte in reversed(float_data))
        print(f"Representação hexadecimal: {hex_representation}")

        # Exibe a representação binária dos dados lidos
        binary_representation = float_to_binary(num)
        print(f"Representação binária: {binary_representation}")
        print(f"Sinal: {binary_representation[0]}")
        print(f"Expoente: {binary_representation[1:9]}")
        print(f"Mantissa: {binary_representation[9:]}")
        print()
