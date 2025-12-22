# Define o compilador
CXX = g++

# Define as flags do compilador
CXXFLAGS = -std=c++17 -O2 -Wall

# Nome do arquivo fonte
SOURCE = src/main.cpp

# Nome do executável final
EXEC = fluxo_maximo

# Regra padrão (all)
all: $(EXEC)

# Regra para compilar e linkar o executável
$(EXEC): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(SOURCE)

# Regra para rodar o programa
run: $(EXEC)
	./$(EXEC)

# Regra para limpar arquivos gerados (executável)
clean:
	rm -f $(EXEC)

# Declara que estas regras não são arquivos
.PHONY: all run clean
