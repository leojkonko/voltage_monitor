# VoltageMonitor

Um aplicativo Qt para monitoramento e visualização gráfica de dados de tensão em tempo real.

## Descrição

VoltageMonitor é uma aplicação que lê valores de tensão de um arquivo JSON e exibe esses dados em um gráfico interativo em tempo real. O aplicativo utiliza a biblioteca QCustomPlot para renderização de gráficos de alta qualidade e oferece recursos como:

- Visualização de dados de tensão em tempo real
- Funcionalidades de zoom e arraste para navegação no gráfico
- Atualização automática dos dados através de um buffer circular
- Suporte para monitoramento contínuo

## Requisitos

- Qt 5.x ou superior
- CMake 3.14 ou superior
- Compilador C++ com suporte a C++17
- nlohmann/json (incluído automaticamente via CMake)
- QCustomPlot 2.1.1 (baixado e configurado automaticamente)

## Compilação

### Compilação local

```bash
mkdir build
cd build
cmake ..
make
```

### Usando Docker

```bash
docker build -t voltage-monitor .
docker run -v ${PWD}:/app/data voltage-monitor /app/data/converted.json
```

Para executar com interface gráfica no Docker, você precisará de um servidor X:

```bash
# No Windows com VcXsrv instalado
docker run -e DISPLAY=host.docker.internal:0 -v ${PWD}:/app/data voltage-monitor /app/data/converted.json
```

## Uso

```bash
./VoltageMonitor <caminho_do_arquivo_json>
```

O arquivo JSON deve ter a seguinte estrutura:

```json
{
  "board_b_12v": {
    "value": ["12.084", "11.656", "11.97", ...],
    "time": []  // Opcional - se não fornecido, tempos sequenciais serão gerados
  }
}
```

## Estrutura do Projeto

- `main.cpp` - Contém a lógica principal e o leitor de dados JSON
- `mainwindow.cpp/h` - Implementação da interface gráfica usando Qt
- `ringbuffer.h` - Um buffer circular para gerenciar os dados entre threads
- `qtmain.cpp` - Arquivo principal para a aplicação Qt
- `CMakeLists.txt` - Arquivo de configuração do CMake
- `Dockerfile` - Configuração para construção e execução em Docker

## Licença

Este projeto é distribuído sob a licença MIT.
