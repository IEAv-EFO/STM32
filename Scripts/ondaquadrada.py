import numpy as np
import matplotlib.pyplot as plt

# Parâmetros
t = np.linspace(0, 6 * np.pi, 1000)  # 3 ciclos completos, onde 2*pi é um ciclo
defasagem = np.pi  # 3/6 do período, 180 graus convertidos em radianos
periodo_parte = np.pi / 3  # 1/6 do período

# Gerando as ondas quadradas
square_wave_1 = np.sign(np.sin(t + defasagem))
square_wave_2 = np.sign(np.sin(t + defasagem + defasagem))
square_wave_3 = np.sign(np.sin(t + defasagem + 2 * defasagem))

# Plotando as ondas quadradas
plt.figure(figsize=(12, 8))

# Define as linhas de grade vertical
x_ticks = np.arange(0, 6 * np.pi + periodo_parte, periodo_parte)

# Função para configurar cada subplot
def configure_plot(ax, data, title, color='b'):
    ax.plot(t, data, color=color)
    ax.set_title(title)
    ax.set_ylim(-1.5, 1.5)
    ax.set_yticks([-1, 1])  # Define os valores de y como -1 (low) e 1 (high)
    ax.set_yticklabels(['Low', 'High'])  # Define os rótulos correspondentes
    ax.axhline(y=0, color='black', linestyle='-', linewidth=1)  # Coloca a linha 'low' sobre o eixo x
    ax.set_xlim(0, 6 * np.pi)
    ax.grid(True, which='both', linestyle='--', linewidth=0.7, alpha=0.7)  # Mantém as linhas de grade
    ax.set_xticks([])  # Remove os rótulos do eixo x

# Plotando cada canal com as configurações definidas
ax1 = plt.subplot(3, 1, 1)
configure_plot(ax1, square_wave_1, 'Canal 1', color='white')

ax2 = plt.subplot(3, 1, 2)
configure_plot(ax2, square_wave_2, 'Canal 2', color='white')

ax3 = plt.subplot(3, 1, 3)
configure_plot(ax3, square_wave_3, 'Canal 3', color='white')

plt.tight_layout()
plt.show()
