# Cliente/Servidor TCP em C++ (Autenticação por Senha)

Projeto simples em C++ que demonstra a comunicação entre um **servidor** e um **cliente** usando **sockets TCP** (API POSIX/BSD). O cliente envia uma senha ao servidor, que verifica se ela está correta e responde com uma mensagem de acesso liberado ou negado.

## Funcionalidades

- Servidor TCP escutando na porta `8080`
- Cliente interativo que lê uma mensagem do terminal e a envia ao servidor
- Verificação de senha no servidor (remove `\n` e `\r` no final da mensagem antes de comparar)
- Tratamento de erros nas chamadas de `socket`, `bind`, `listen`, `accept`, `send` e `recv`
- Uso de `SO_REUSEADDR` para reiniciar o servidor rapidamente sem esperar o timeout da porta

## Estrutura do projeto

```
.
├── server.cpp   # Servidor TCP
├── client.cpp   # Cliente TCP
└── README.md
```

## Requisitos

- Sistema **Linux** ou **macOS** (usa headers POSIX como `<sys/socket.h>` e `<netinet/in.h>`)
- Compilador C++ com suporte a C++11 ou superior (`g++` ou `clang++`)

> No Windows, use o **WSL** ou adapte o código para Winsock.

## Compilação

```bash
g++ -std=c++11 -Wall -o server server.cpp
g++ -std=c++11 -Wall -o client client.cpp
```

## Como executar

**1. Inicie o servidor** em um terminal:

```bash
./server
```

**2. Execute o cliente** em outro terminal:

```bash
./client
```

**3. Digite a senha** quando solicitado:

```
Enter your message to server: 12345
Resposta do servidor: you enter the right password
```

Com uma senha incorreta:

```
Enter your message to server: abc
Resposta do servidor: wrong password, access denied
```

Saída do servidor durante o uso:

```
new client created
Message from client: 12345
```

Para encerrar o servidor, use `Ctrl + C`.

## Como funciona

### Servidor (`server.cpp`)

1. Cria um socket TCP (`socket`) e habilita `SO_REUSEADDR`
2. Associa o socket à porta `8080` em todas as interfaces (`bind` com `INADDR_ANY`)
3. Fica em modo de escuta (`listen`, backlog de 100)
4. Em loop infinito:
   - Aceita uma conexão (`accept`)
   - Recebe uma mensagem (`recv`, buffer de 1024 bytes)
   - Compara com a senha esperada e envia a resposta (`send`)
   - Fecha a conexão com o cliente e volta a aguardar o próximo

| Função | Descrição |
|---|---|
| `bindingSocket` | Faz o `bind` do socket ao endereço/porta e trata erros |
| `sendingData` | Envia dados ao cliente, validando se a string não está vazia |
| `processMessageFromClient` | Limpa quebras de linha e valida a senha |

### Cliente (`client.cpp`)

1. Cria um socket TCP
2. Conecta ao servidor em `127.0.0.1:8080`
3. Lê uma linha do terminal (`std::getline`) e envia ao servidor
4. Aguarda e exibe a resposta do servidor
5. Fecha a conexão

## Protocolo

| Cliente envia | Servidor responde |
|---|---|
| `12345` | `you enter the right password` |
| Qualquer outra coisa | `wrong password, access denied` |

Cada conexão troca **uma única mensagem** e é encerrada em seguida.

## Configuração

Os valores abaixo estão fixos no código. Para alterá-los, edite os arquivos e recompile:

| Parâmetro | Valor | Onde |
|---|---|---|
| Porta | `8080` | `server.cpp` e `client.cpp` |
| Endereço do servidor | `127.0.0.1` | `client.cpp` |
| Senha | `12345` | `server.cpp` (`processMessageFromClient`) |
| Tamanho do buffer | `1024` bytes | `server.cpp` e `client.cpp` |

Para conectar de outra máquina da rede, troque `127.0.0.1` em `client.cpp` pelo IP do servidor.

## Limitações

- **Senha fixa no código-fonte** e sem hash
- **Comunicação sem criptografia** (texto puro); a senha trafega legível pela rede
- **Sem proteção contra força bruta** (sem limite de tentativas)
- **Servidor atende um cliente por vez** (iterativo, sem threads ou `select`/`poll`)
- Cada conexão processa **apenas uma mensagem**
- Uma única chamada a `recv` assume que a mensagem chega inteira

## Ideias de melhoria

- Atender múltiplos clientes com `std::thread`, `select`, `poll` ou `epoll`
- Ler porta, IP e senha por argumentos de linha de comando ou variáveis de ambiente
- Armazenar a senha como hash (ex.: bcrypt/Argon2) em vez de texto puro
- Adicionar TLS (ex.: OpenSSL) para criptografar a comunicação
- Implementar limite de tentativas e logs de acesso
- Tratar sinais (`SIGINT`) para encerrar o servidor de forma limpa
- Criar um `Makefile` ou `CMakeLists.txt`

