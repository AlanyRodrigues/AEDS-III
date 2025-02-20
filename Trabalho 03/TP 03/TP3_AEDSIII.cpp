#include <windows.h>

#include <fstream>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

struct Coordenada {
    int x;
    int y;
};

struct Aresta {
    int vertice;
    int peso;
};

bool direcionado;
int nVertices;
vector<Coordenada> coordenadas;
vector<vector<Aresta>> adjacencias;
vector<string> nomesVertices;

vector<int> desc, term, ant, cor;
vector<int> distancia;
int tempo;

void limparGrafo() {
    coordenadas.clear();
    adjacencias.clear();
    nomesVertices.clear();
}

void criarVazio(bool dir, int vertices) {
    direcionado = dir;
    nVertices = vertices;
    adjacencias.resize(vertices);
    coordenadas.resize(vertices);
    nomesVertices.resize(vertices);
}

void inserirAresta(int origem, int destino, int peso) {
    adjacencias[origem].push_back({destino, peso});
    if (!direcionado) {
        adjacencias[destino].push_back({origem, peso});
    }
}

void removerAresta(int origem, int destino) {
    for (int i = 0; i < adjacencias[origem].size(); i++) {
        if (adjacencias[origem][i].vertice == destino) {
            adjacencias[origem].erase(adjacencias[origem].begin() + i);
            break;
        }
    }

    if (!direcionado) {
        for (int i = 0; i < adjacencias[destino].size(); i++) {
            if (adjacencias[destino][i].vertice == origem) {
                adjacencias[destino].erase(adjacencias[destino].begin() + i);
                break;
            }
        }
    }
}

bool consultarPrimeiroAdjacente(int vertice) {
    // aponta para o par correspondente ao vértice no mapa
    if (!adjacencias.empty()) {
        cout << "Primeiro adjacente: " << adjacencias[vertice][0].vertice
             << endl;
        return true;
    } else {
        cout << "O vértice " << vertice << " não possui adjacências.\n";
    }
}

bool consultarProximoAdjacente(int vertice, int adjacenteAtual) {
    bool encontrado = false;
    for (int i = 0; i < adjacencias[vertice].size(); i++) {
        if (adjacencias[vertice][i].vertice == adjacenteAtual &&
            i + 1 < adjacencias[vertice].size()) {
            cout << "Próximo adjacente: " << adjacencias[vertice][i + 1].vertice
                 << endl;
            encontrado = true;
            break;
        }
    }
    if (!encontrado) {
        cout << "Nenhum próximo adjacente encontrado ";
    }
}

void consultarListaAdjacentes(int vertice) {
    if (!adjacencias[vertice].empty()) {
        cout << "Lista de adjacentes do vértice " << vertice << ":";
        for (int i = 0; i < adjacencias[vertice].size(); i++) {
            // acessa o vértice adjacente na estrutura Aresta
            cout << adjacencias[vertice][i].vertice << " ";
        }
        cout << endl;
    } else {
        cout << "Nenhum adjacente encontrado\n";
    }
}

void editarCoordenada(int vertice, int x, int y) {
    if (vertice >= 0 && vertice < nVertices) {
        coordenadas[vertice] = {x, y};
        cout << "Coordenada do vértice " << vertice << " atualizada para (" << x
             << ", " << y << ")\n";
    } else {
        cout << "Vértice não encontrado\n";
    }
}

void exibirAdjacencias() {
    for (int i = 0; i < nVertices; i++) {
        cout << "Vértice " << i << " (" << nomesVertices[i] << "): ";

        if (!adjacencias[i].empty()) {
            for (int j = 0; j < adjacencias[i].size(); j++) {
                cout << " -> " << adjacencias[i][j].vertice;
            }
        } else {
            cout << " Não foram encontradas adjacências";
        }
        cout << endl;
    }
}

void editarNomeVertice() {
    int vertice;
    string novoNome;

    cout << "Informe o numero do vértice: ";
    cin >> vertice;
    cin.ignore();

    if (vertice < 0 || vertice >= nVertices) {
        cout << "Índice inválido!\n";
        return;
    }

    cout << "Informe o novo nome para o vértice: ";
    cin >> novoNome;

    nomesVertices[vertice] = novoNome;
    cout << "Nome do vértice atualizado com sucesso!\n";
}

void exportarGrafo(const char* arquivo) {
    ofstream file(arquivo);
    if (!file.is_open()) {
        cout << "Erro ao abrir o arquivo!" << endl;
        return;
    }

    file << "direcionado=" << (direcionado ? "sim" : "nao") << endl;
    file << nVertices << endl;

    for (int i = 0; i < nVertices; i++) {
        file << i << " " << coordenadas[i].x << " " << coordenadas[i].y << " " << nomesVertices[i] << endl;
    }

    vector<pair<int, int>> arestasExpo;    // armazena as arestas
    int nArestas = 0;

    for (int i = 0; i < adjacencias.size(); i++) {
        for (int j = 0; j < adjacencias[i].size(); j++) {
            int origem = i;
            int destino = adjacencias[i][j].vertice;
            if (direcionado || origem < destino) {
                arestasExpo.push_back({origem, destino});
                nArestas++;
            }
        }
    }

    file << nArestas << endl;

    for (int i = 0; i < arestasExpo.size(); i++) {
        int origem = arestasExpo[i].first;
        int destino = arestasExpo[i].second;
        for (int j = 0; j < adjacencias[origem].size(); j++) {
            if (adjacencias[origem][j].vertice == destino) {
                file << origem << " " << destino << " " << adjacencias[origem][j].peso << endl;
                break;
            }
        }
    }
    file.close();
    cout << "Grafo exportado com sucesso!\n";
}

void importarGrafo(const char* arquivo) {
    ifstream file(arquivo);
    if (!file.is_open()) {
        cout << "Erro ao abrir o arquivo!" << endl;
        return;
    }

    string linha;
    getline(file, linha);
    direcionado = (linha.find("sim") != string::npos);

    file >> nVertices;
    coordenadas.resize(nVertices);
    adjacencias.resize(nVertices);
    nomesVertices.resize(nVertices);

    for (int i = 0; i < nVertices; i++) {
        int id, x, y;
        string nome;
        file >> id >> x >> y;
        getline(file, nome);
        if (!nome.empty() && nome[0] == ' ') nome.erase(0, 1);
        coordenadas[id] = {x, y};
        nomesVertices[id] = nome;
    }

    int nArestas;
    file >> nArestas;

    for (int i = 0; i < nArestas; i++) {
        int origem, destino, peso;
        file >> origem >> destino >> peso;
        inserirAresta(origem, destino, peso);
    }

    file.close();
    cout << "Grafo importado com sucesso!\n";
}

void visitaEmProfundidade(int vertice) {
    cor[vertice] = 1;         // marca o vértice como em processamento
    desc[vertice] = ++tempo;  // marca o tempo de descoberta
    cout << "Descoberta do vértice " << vertice << " ("
         << nomesVertices[vertice] << ") no tempo " << tempo << "\n";

    for (const auto& adj : adjacencias[vertice]) {
        int v = adj.vertice;
        if (cor[v] == 0) {  // se o vértice não foi visitado
            ant[v] = vertice;
            visitaEmProfundidade(v);
        }
    }

    cor[vertice] = 2;         // marca o vértice como finalizado
    term[vertice] = ++tempo;  // marca o tempo de finalização
    cout << "Finalização do vértice " << vertice << " (" << nomesVertices[vertice] << ") no tempo " << tempo << "\n";
}

void buscaEmProfundidade() {
    // inicializa os vetores antes de iniciar a busca
    fill(desc.begin(), desc.end(), 0);    // zera todos os vetores
    fill(term.begin(), term.end(), 0);
    fill(ant.begin(), ant.end(), -1);
    fill(cor.begin(), cor.end(), 0);    // define como não visitados
    tempo = 0;    // reinicia o tempo

    for (int i = 0; i < nVertices; i++) {
        if (cor[i] == 0) {  // se o vértice não foi visitado
            visitaEmProfundidade(i);
        }
    }
}

void visitaEmLargura(int vertice) {
    queue<int> fila;
    cor[vertice] = 1;
    distancia[vertice] = 0;
    fila.push(vertice);

    cout << "Descoberta do vértice " << vertice << " ("
         << nomesVertices[vertice] << ")\n";

    while (!fila.empty()) {
        int v = fila.front();
        fila.pop();  // remove o primeiro elemento da fila

        for (const auto& adj : adjacencias[v]) {
            int k = adj.vertice;
            if (cor[k] == 0) {
                cor[k] = 1;
                ant[k] = v;
                distancia[k] = distancia[v] + 1;
                fila.push(k);
                cout << "Descoberta do vértice " << k << " (" << nomesVertices[k] << ") com distância " << distancia[k] << "\n";
            }
        }
        cor[v] = 2;
        cout << "Finalização do vértice " << v << " (" << nomesVertices[v] << ")\n";
    }
}

void buscaEmLargura() {
    fill(ant.begin(), ant.end(), -1);    // define todos os antecessores como -1
    fill(cor.begin(), cor.end(), 0);    // define todos os vértices como não visitados
    fill(distancia.begin(), distancia.end(), 1000);    // define todas as distâncias como 1000

    for (int i = 0; i < nVertices; i++) {
        if (cor[i] == 0) {
            visitaEmLargura(i);
        }
    }
}

void prim() {
    //inicialização
    vector<int> menorPeso(nVertices, 1000);
    vector<int> verticePai(nVertices, -1);
    vector<bool> estaNoCirculo(nVertices, false);

    menorPeso[0] = 0;

    for (int i = 0; i < nVertices - 1; i++)
    {
        int valorMin = 1000, u = -1;

        for (int v = 0; v < nVertices; v++)
        {
            if (!estaNoCirculo[v] && menorPeso[v] < valorMin)
            {
                valorMin = menorPeso[v];
                u = v;
            }
        }

        if (u == -1)
            break;

        estaNoCirculo[u] = true;
        cout << "Vértice " << u << " adicionado a circulo.\n";

        for (int j = 0; j < adjacencias[u].size(); j++) // atualiza os pesos
        {
            int v = adjacencias[u][j].vertice;
            int peso = adjacencias[u][j].peso;

            if (!estaNoCirculo[v] && peso < menorPeso[v])
            {
                menorPeso[v] = peso;
                verticePai[v] = u;
                cout << "Atualizando aresta com menor peso do vértice " << v << " para " << peso << "\n";
            }
        }
    }

    cout << "\nÁrvore Geradora Mínima (Prim):\n";
    int pesoTotal = 0;
    for (int v = 1; v < nVertices; v++)
    {
        if (verticePai[v] != -1)
        {
            cout << "Aresta: (" << verticePai[v] << " - " << v << ") com peso " << menorPeso[v] << "\n";
            pesoTotal += menorPeso[v];
        }
    }
    cout << "\nPeso total da árvore: " << pesoTotal << "\n";
}

void dijkstra(string nomeOrigem, string nomeDestino) {
    int origem = -1, destino = -1;
    for (int i = 0; i < nVertices; i++)
    {
        if (nomesVertices[i] == nomeOrigem)
            origem = i;
        if (nomesVertices[i] == nomeDestino)
            destino = i;
    }
    if (origem == -1 || destino == -1)
    {
        cout << "Vértices não encontrados!\n";
        return;
    }

    // inicialização
    int infinito = 1000;
    vector<int> dist(nVertices, infinito);
    vector<int> antecessor(nVertices, -1);
    vector<bool> visitado(nVertices, false);

    dist[origem] = 0;

    for (int i = 0; i < nVertices; i++) // processamento dos vértices
    {
        int u = -1;
        for (int j = 0; j < nVertices; j++)
        {
            if (!visitado[j] && (u == -1 || dist[j] < dist[u]))
                u = j;
        }
        if (dist[u] == infinito)
            break;
        visitado[u] = true;

        cout << "Processando vértice " << u << " (" << nomesVertices[u] << "). Distância: " << dist[u] << "\n";

        for (int j = 0; j < adjacencias[u].size(); j++)
        {
            int v = adjacencias[u][j].vertice, peso = adjacencias[u][j].peso;
            if (!visitado[v] && dist[u] + peso < dist[v])
            {
                dist[v] = dist[u] + peso;
                antecessor[v] = u;
                cout << "Vértice " << v << " encontrado: " << "(" << nomesVertices[v] << "). Com peso: " << dist[v] << "\n";
            }
        }
        cout << endl;
    }

    if (dist[destino] == infinito)
    {
        cout << "Não há caminho entre os vértices.\n";
        return;
    }

    cout << "\nMenor caminho de " << nomeOrigem << " para " << nomeDestino << ":\n";
    vector<int> caminho;
    for (int v = destino; v != -1; v = antecessor[v])
    {
        caminho.push_back(v);
    }

    for (int i = 0; i < caminho.size() / 2; i++)
    {
        int temp = caminho[i];
        caminho[i] = caminho[caminho.size() - 1 - i];
        caminho[caminho.size() - 1 - i] = temp;
    }

    for (int i = 0; i < caminho.size(); i++)
    {
        if (i > 0)
            cout << " -> ";
        cout << caminho[i] << " (" << nomesVertices[caminho[i]] << ")";
    }
    cout << "\nDistância total: " << dist[destino] << "\n";
}

void menu() {
    cout << "\n------------------------- MENU -------------------------\n";
    cout << "01. Criar grafo vazio\n";
    cout << "02. Inserir aresta\n";
    cout << "03. Inserir arestas em conjunto\n";
    cout << "04. Remover aresta\n";
    cout << "05. Consultar primeiro adjacente de um vértice\n";
    cout << "06. Consultar próximo adjacente de um vértice\n";
    cout << "07. Consultar lista completa de adjacentes de um vértice\n";
    cout << "08. Editar as coordenadas de um vértice\n";
    cout << "09. Exibir adjacências\n";
    cout << "10. Editar nome do vértice\n";
    cout << "11. Exportar grafo\n";
    cout << "12. Importar grafo\n";
    cout << "13. Busca em profundidade\n";
    cout << "14. Busca em largura\n";
    cout << "15. Árvore gerafora mínima (Prim)\n";
    cout << "16. Menor caminho (Dijkstra)\n";
    cout << "0. Sair\n\n";
    cout << "Escolha uma opção: ";
}

int main() {
    UINT CPAGE_UTF8 = 65001;
    SetConsoleOutputCP(CPAGE_UTF8);

    int opcao;

    while (true) {
        system("cls");
        menu();
        cin >> opcao;

        if (opcao == 0) break;

        system("cls");
        switch (opcao) {
            case 1: {
                limparGrafo();

                int vertices;
                char op;

                cout << "O grafo será direcionado? (s/n): ";
                cin >> op;

                bool direcionado = (op == 's');

                cout << "Informe o número de vértices: ";
                cin >> vertices;

                criarVazio(direcionado, vertices);
                for (int i = 0; i < vertices; ++i) {
                    cout << "Informe o nome do vértice " << i << ": ";
                    cin >> nomesVertices[i];
                }
                
                cout << "\nGrafo vazio" << (direcionado ? " direcionado " : " não direcionado ") << "criado com " << vertices << " vértices.\n";
                break;
            }
            case 2: {
                int origem, destino, peso;
                cout << "Informe a origem, destino e peso: ";
                cin >> origem >> destino >> peso;
                inserirAresta(origem, destino, peso);
                cout << "Aresta inserida com sucesso!\n";
                break;
            }
            case 3: {
                int origem, destino, peso, quantidade;

                cout << "Informe o número de arestas: ";
                cin >> quantidade;

                for (int i = 0; i < quantidade; i++) {
                    cout << "Informe a origem, o destino e o peso da aresta: ";
                    cin >> origem >> destino >> peso;

                    // verifica se os índices de origem e destino estão dentro do intervalo válido
                        adjacencias[origem].push_back({destino, peso});
                        if (!direcionado) {
                            adjacencias[destino].push_back({origem, peso});
                        }
                    }

                cout << "Arestas inseridas com sucesso!\n";
                break;
            }
            case 4: {
                int origem, destino;
                cout << "Informe o vértice de origem: ";
                cin >> origem;
                cout << "Informe o vértice de destino: ";
                cin >> destino;
                removerAresta(origem, destino);
                cout << "Aresta removida com sucesso.\n";
                break;
            }
            case 5: {
                int vertice;
                cout << "Informe o vértice: ";
                cin >> vertice;
                consultarPrimeiroAdjacente(vertice);
                break;
            }
            case 6: {
                int vertice, atual;
                cout << "Informe o vértice e o adjacente atual: ";
                cin >> vertice >> atual;
                consultarProximoAdjacente(vertice, atual);
                break;
            }
            case 7: {
                int vertice;
                cout << "Informe o vértice: ";
                cin >> vertice;
                consultarListaAdjacentes(vertice);
                break;
            }
            case 8: {
                int vertice, x, y;
                cout << "Informe o vértice e as novas coordenadas (x e y): ";
                cin >> vertice >> x >> y;
                editarCoordenada(vertice, x, y);
                break;
            }
            case 9: {
                exibirAdjacencias();
                break;
            }
            case 10: {
                editarNomeVertice();
                break;
            }
            case 11: {
                char arquivo[256];
                cout << "Informe o nome do arquivo para exportação: ";
                cin >> arquivo;
                exportarGrafo(arquivo);
                break;
            }
            case 12: {
                limparGrafo();
                char arquivo[256];
                cout << "Informe o nome do arquivo para importação: ";
                cin >> arquivo;
                importarGrafo(arquivo);
                break;
            }
            case 13: {
                cout << "Busca em profundidade:\n";
                buscaEmProfundidade();
                break;
            }
            case 14: {
                cout << "Busca em largura:\n";
                buscaEmLargura();
                break;
            }
            case 15: {
                prim();
                break;
            }
            case 16: {
                string origem, destino;
                cout << "Informe o nome dos vértice de origem: ";
                cin.ignore();
                getline(cin, origem);

                cout << "Informe o nome dos vértice de destino: ";
                getline(cin, destino);
                cout << endl;

                dijkstra(origem, destino);
                break;
            }

            default:
            cout << "Opção inválida!\n";
        }
        system("pause");
    }

    cout << "Encerrando...\n";

    return 0;
}
