#include <iostream>
#include <windows.h>
#include <vector> // gerencia listas de elementos
#include <map> // armazena dados de forma que cada chave é única
#include <fstream> // manipula arquivos
#include <tuple> // permite criar objetos que podem armazenar múltiplos valores de diferentes tipos
#include <algorithm> // fornece funções para operações em containers

using namespace std;

// representa a coordenada
struct Coordenada
{
    float x, y;
};

// representa a conexão entre dois vértices
struct Aresta
{
    int destino;
    float peso;
};

class Grafo
{
private:
    bool direcionado; //indica se é direcionado ou não
    map<int, Coordenada> vertices; // mapa de vértices
    map<int, vector<Aresta>> adjacencias; // mapa de adjacências

public:
    // inicializa o grafo
    Grafo(bool direcional = false) : direcionado(direcional) {}

    // cria uma lista de adjacências vazia
    void criarVazio(int n)
    {
        vertices.clear();
        adjacencias.clear();
        for (int i = 0; i < n; ++i)
        {
            vertices[i] = {0, 0};
            adjacencias[i] = {};
        }
    }

    // exibe todas as adjacências dos vértice
    void exibirAdjacencias() const
    {
        if (adjacencias.empty())
        {
            cout << "O grafo não possui adjacências.\n";
            return;
        }

        for (const auto &par : adjacencias)
        {
            int vertice = par.first; // vértice atual
            const auto &lista = par.second; // lista de adjacências

            cout << "Vértice " << vertice << " (";
            if (vertices.find(vertice) != vertices.end())
            {
                cout << "Coordenadas: " << vertices.at(vertice).x << ", " << vertices.at(vertice).y;
            }
            else
            {
                cout << "Coordenadas não definidas";
            }
            cout << "): ";

            if (lista.empty())
            {
                cout << "Nenhuma adjacência.\n";
            }
            else
            {
                for (const auto &aresta : lista)
                {
                    cout << "(" << aresta.destino << ", peso: " << aresta.peso << ") ";
                }
                cout << "\n";
            }
        }
    }

    // adiciona uma aresta entre os vértices
    void inserirAresta(int origem, int destino, float peso)
    {
        adjacencias[origem].push_back({destino, peso});
        if (!direcionado)
        {
            adjacencias[destino].push_back({origem, peso});
        }
    }

    // adiciona várias arestas entre os vértices
    void inserirArestasEmConjunto()
    {
        int numArestas;
        cout << "Quantas arestas deseja inserir? ";
        cin >> numArestas;

        for (int i = 0; i < numArestas; ++i)
        {
            int origem, destino;
            float peso;
            cout << "Aresta " << i + 1 << ":\n";
            cout << "Origem: ";
            cin >> origem;
            cout << "Destino: ";
            cin >> destino;
            cout << "Peso: ";
            cin >> peso;
            inserirAresta(origem, destino, peso);
            cout << "\n"; 
        }
        cout << numArestas << " arestas inseridas com sucesso.\n";
        system("pause");
        system("cls");
    }

    // remove a aresta
    void removerAresta(int origem, int destino)
    {
        auto &lista = adjacencias[origem]; // retorna uma lista de adjacências associadas a origem
        lista.erase(remove_if(lista.begin(), lista.end(), // identifica os elementos da lista que atendem a condição
                              // verifica se o destino a é igual ao fornecido  
                              [destino](const Aresta &a) 
                              { return a.destino == destino; }),
                    lista.end());

        if (!direcionado)
        {
            auto &lista_reversa = adjacencias[destino];
            lista_reversa.erase(remove_if(lista_reversa.begin(), lista_reversa.end(), // remove a aresta em que destino é igual ao origem
                                               [origem](const Aresta &a)
                                               { return a.destino == origem; }),
                                lista_reversa.end());
        }
    }

    // retorna o primeiro adjacente de um vértice
    bool consultarPrimeiroAdjacente(int vertice, Aresta &resultado) const
    {
        // aponta para o par correspondente ao vértice no mapa
        auto it = adjacencias.find(vertice);
        if (it != adjacencias.end() && !it->second.empty())
        {
            resultado = it->second.front(); // pega o primeiro adjacente
            return true;
        }
        return false;
    }

    // retorna o próximo adjacente após um adjacente atual
    bool consultarProximoAdjacente(int vertice, int adjacenteAtual, Aresta &resultado) const
    {
        auto it = adjacencias.find(vertice);
        if (it != adjacencias.end())
        {
            auto &lista = it->second;
            auto adjacente = find_if(lista.begin(), lista.end(), // busca a aresta que tem como destino o adjacente atual
                                          [adjacenteAtual](const Aresta &a)
                                          { return a.destino == adjacenteAtual; });
            if (adjacente != lista.end() && next(adjacente) != lista.end()) // se o adjacente foi encontrado e existe um próximo na lista, continua
            {
                resultado = *next(adjacente); // próximo adjacente é armazenado
                return true;
            }
        }
        return false;
    }

    // retorna todas as arestas adjacentes a um vértice
    vector<Aresta> consultarListaAdjacentes(int vertice) const
    {
        auto it = adjacencias.find(vertice);
        if (it != adjacencias.end())
        {
            return it->second; // retorna a lista de adjacentes
        }
        return {};
    }

    // Atualiza as coordenadas de um vértice específico
    void editarCoordenada(int vertice, float x, float y)
    {
        if (vertices.find(vertice) != vertices.end())
        {
            vertices[vertice] = {x, y}; // atualiza as coordenadas
        }
        else
        {
            cout << "Vértice não encontrado.\n";
        }
    }

    // exporta o grafo para um arquivo
    void exportarGrafo(const string &arquivo) const
    {
        ofstream out(arquivo);
        if (!out.is_open())
        {
            out << "Erro ao abrir o arquivo para escrita.\n";
            return;
        }

        out << "direcionado=" << (direcionado ? "sim" : "não") << "\n";
        out << vertices.size() << "\n";

        // para cada par de vértice o id e as coordenadas são escritos no arquivo
        for (const auto &par : vertices)
        {
            int id = par.first;
            const auto &coord = par.second;
            out << id << " " << coord.x << " " << coord.y << "\n";
        }

        // calcula o número de arestas
        int nArestas = 0;
        for (const auto &par : adjacencias)
        {
            nArestas += par.second.size();
        }
        if (!direcionado) // se o grafo não for direcionado, o número é dividido por 2 para evitar contagem dupla
            nArestas /= 2;

        out << nArestas << "\n";

        // para cada vértice, a função itera sobre sua lista de arestas
        for (const auto &par : adjacencias)
        {
            int origem = par.first;
            const auto &lista = par.second;

            for (const auto &aresta : lista)
            {
                if (!direcionado && origem > aresta.destino)
                    continue; // evita duplicação em grafos não direcionados
                out << origem << " " << aresta.destino << " " << aresta.peso << "\n";
            }
        }
    }

    // lê a estrutura do grafo de um arquivo
    void importarGrafo(const string &arquivo)
    {
        ifstream in(arquivo);
        if (!in.is_open())
        {
            cout << "Erro ao abrir o arquivo.\n";
            return;
        }

        string linha;
        getline(in, linha);
        direcionado = (linha.find("sim") != string::npos);

        int nVertices; // número de vértices é lido a partir do arquivo
        in >> nVertices;

        // lê cada vértice
        for (int i = 0; i < nVertices; ++i) 
        {
            int id;
            float x, y;
            in >> id >> x >> y;
            vertices[id] = {x, y}; // adiciona o vértice e suas coordenadas
            adjacencias[id] = {}; // inicializa a lista de adjacências
        }

        int nArestas;
        in >> nArestas;

        // lê cada aresta
        for (int i = 0; i < nArestas; ++i)
        {
            int origem, destino;
            float peso;
            in >> origem >> destino >> peso;
            inserirAresta(origem, destino, peso); // insere as arestas
        }
    }
};

// apresenta o menu
void menu()
{
    Grafo grafo(false);
    int opcao;

    do
    {
        cout << "\n------------MENU------------\n";
        cout << "1. Criar grafo vazio\n";
        cout << "2. Inserir aresta\n";
        cout << "3. Inserir arestas em conjunto\n";
        cout << "4. remover aresta\n";
        cout << "5. Consultar primeiro adjacente de um vértice\n";
        cout << "6. Consultar próximo adjacente de um vértice\n";
        cout << "7. Consultar lista completa de adjacentes de um vértice\n";
        cout << "8. Editar coordenada de um vértice\n";
        cout << "9. Exibir adjacências\n";
        cout << "10. Exportar grafo\n";
        cout << "11. Importar grafo\n";
        cout << "0. Sair\n\n";
        cout << "Escolha uma opção: ";
        cin >> opcao;

        system("cls");

        switch (opcao)
        {
        case 1:
        {
            int n;
            cout << "Informe o número de vértices: ";
            cin >> n;
            grafo.criarVazio(n);
            cout << "Grafo criado com " << n << " vértices.\n";
            system("pause");
            system("cls");
            break;
        }
        case 2:
        {
            int origem, destino;
            float peso;
            cout << "Informe o vértice de origem: ";
            cin >> origem;
            cout << "Informe o vértice de destino: ";
            cin >> destino;
            cout << "Informe o peso da aresta: ";
            cin >> peso;
            grafo.inserirAresta(origem, destino, peso);
            cout << "Aresta inserida com sucesso.\n";
            system("pause");
            system("cls");
            break;
        }
        case 3:
            grafo.inserirArestasEmConjunto();
            break;
        case 4:
        {
            int origem, destino;
            cout << "Informe o vértice de origem: ";
            cin >> origem;
            cout << "Informe o vértice de destino: ";
            cin >> destino;
            grafo.removerAresta(origem, destino);
            cout << "Aresta removida com sucesso.\n";
            system("pause");
            system("cls");
            break;
        }
        case 5:
        {
            int vertice;
            cout << "Informe o vértice: ";
            cin >> vertice;
            Aresta adjacente;
            if (grafo.consultarPrimeiroAdjacente(vertice, adjacente))
            {
                cout << "Primeiro adjacente: " << adjacente.destino << " (peso: " << adjacente.peso << ")\n";
                system("pause");
                system("cls");
            }
            else
            {
                cout << "Nenhum adjacente encontrado.\n";
                system("pause");
                system("cls");
            }
            break;
        }
        case 6:
        {
            int vertice, adjacenteAtual;
            cout << "Informe o vértice: ";
            cin >> vertice;
            cout << "Informe o adjacente atual: ";
            cin >> adjacenteAtual;
            Aresta proximoAdjacente;
            if (grafo.consultarProximoAdjacente(vertice, adjacenteAtual, proximoAdjacente))
            {
                cout << "Próximo adjacente: " << proximoAdjacente.destino << " (peso: " << proximoAdjacente.peso << ")\n";
                system("pause");
                system("cls");
            }
            else
            {
                cout << "Nenhum próximo adjacente encontrado.\n";
                system("pause");
                system("cls");
            }
            break;
        }
        case 7:
        {
            int vertice;
            cout << "Informe o vértice: ";
            cin >> vertice;
            auto adjacentes = grafo.consultarListaAdjacentes(vertice);
            if (!adjacentes.empty())
            {
                cout << "Adjacentes de " << vertice << ":\n";
                for (const auto &adj : adjacentes)
                {
                    cout << "- Vértice " << adj.destino << " (peso: " << adj.peso << ")\n";
                    system("pause");
                    system("cls");
                }
            }
            else
            {
                cout << "Nenhum adjacente encontrado.\n";
                system("pause");
                system("cls");
            }
            break;
        }
        case 8:
        {
            int vertice;
            float x, y;
            cout << "Informe o vértice: ";
            cin >> vertice;
            cout << "Informe a nova coordenada x: ";
            cin >> x;
            cout << "Informe a nova coordenada y: ";
            cin >> y;
            grafo.editarCoordenada(vertice, x, y);
            cout << "Coordenadas editadas com sucesso.\n";
            system("pause");
            system("cls");
            break;
        }
        case 9:
            grafo.exibirAdjacencias();
            system("pause");
            system("cls");
            break;
        case 10:
        {
            string arquivo;
            cout << "Informe o nome do arquivo para exportação: ";
            cin >> arquivo;
            grafo.exportarGrafo(arquivo);
            cout << "Grafo exportado para " << arquivo << " com sucesso.\n";
            system("pause");
            system("cls");
            break;
        }
        case 11:
        {
            string arquivo;
            cout << "Informe o nome do arquivo para importação: ";
            cin >> arquivo;
            grafo.importarGrafo(arquivo);
            cout << "Grafo importado com sucesso.\n";
            system("pause");
            system("cls");
            break;
        }
        case 0:
            cout << "Encerrando o programa...\n";
            break;
        default:
            cout << "Opção inválida. Tente novamente.\n";
            system("pause");
            system("cls");
        }
    } while (opcao != 0);
}

int main()
{
    UINT CPAGE_UTF8 = 65001;
    SetConsoleOutputCP(CPAGE_UTF8);

    menu(); // chama a função de menu

    return 0;
}
