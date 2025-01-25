#include <iostream>
#include <windows.h>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <string>

using namespace std;

struct Registro {
    char valor;
    int fita_index;
    bool operator<(const Registro& outro) const {
        return valor > outro.valor;  // Para criar um heap mínimo
    }
};

// distribui blocos ordenados nas fitas de saída
void distribuicao_inicial(ifstream& entrada, vector<ofstream>& fitas_saida, int memoria_interna) {
    string bloco;
    char valor;
    int fita_atual = 0;
    // lê os dados e distribui em blocos ordenados nas fitas
    while (entrada >> valor) {
        bloco += valor;
        // quando o bloco atinge o tamanho da memória interna, ordena e escreve na fita
        if (bloco.size() == memoria_interna) {
            sort(bloco.begin(), bloco.end());
            fitas_saida[fita_atual] << bloco << '\n';  // escreve o bloco inteiro
            fita_atual = (fita_atual + 1) % fitas_saida.size();  // alterna entre fitas
            bloco.clear();
        }
    }
    // escreve o último bloco caso tenha restado algum
    if (!bloco.empty()) {
        sort(bloco.begin(), bloco.end());
        fitas_saida[fita_atual] << bloco << '\n';
    }
}

// realiza a intercalação balanceada dos blocos das fitas de entrada para a fita de saída
void intercalacao(vector<ifstream>& fitas_entrada, ofstream& fita_saida) {
    priority_queue<Registro> heap;
    vector<char> valores_atual(fitas_entrada.size(), '\0');

    // inicializa o heap com o primeiro valor de cada fita
    for (size_t i = 0; i < fitas_entrada.size(); ++i) {
        if (fitas_entrada[i] >> valores_atual[i]) {
            heap.push({valores_atual[i], static_cast<int>(i)});
        }
    }

    // intercala até o heap estar vazio
    while (!heap.empty()) {
        Registro menor = heap.top();
        heap.pop();

        fita_saida << menor.valor;  // escreve o menor valor na fita de saída

        int fita_index = menor.fita_index;
        if (fitas_entrada[fita_index] >> valores_atual[fita_index]) {
            heap.push({valores_atual[fita_index], fita_index});
        }
    }
    fita_saida << '\n';  // delimitador de fase
}

// função principal para a ordenação externa
void ordenacao_externa(const string& nome_arquivo_entrada, const string& nome_arquivo_saida, int memoria_interna, int num_fitas) {
    ifstream arquivo_entrada(nome_arquivo_entrada);
    vector<string> nomes_fitas;
    vector<ofstream> fitas_saida;

    // criar os nomes dos arquivos das fitas
    for (int i = 0; i < num_fitas; ++i) {
        nomes_fitas.push_back("fita_" + to_string(i) + ".txt");
        fitas_saida.emplace_back(nomes_fitas[i]);
    }

    // realiza a distribuição inicial nas fitas
    distribuicao_inicial(arquivo_entrada, fitas_saida, memoria_interna);
    arquivo_entrada.close();

    for (auto& fita : fitas_saida) {
        fita.close();
    }

    bool ordenado = false;
    int fase = 0;

    // loop para as fases de intercalação até a ordenação final
    while (!ordenado) {
        vector<ifstream> fitas_entrada;
        vector<ofstream> fitas_saida_fase;

        // abre as fitas de entrada e saída para a fase
        for (int i = 0; i < num_fitas; ++i) {
            if (i < num_fitas / 2) {
                fitas_entrada.emplace_back(nomes_fitas[i]);
            } else {
                fitas_saida_fase.emplace_back(nomes_fitas[i]);
            }
        }

        // executa a intercalação balanceada nesta fase
        for (auto& fita_saida : fitas_saida_fase) {
            intercalacao(fitas_entrada, fita_saida);
        }

        // fecha as fitas de entrada e saída da fase
        for (auto& fita : fitas_entrada) {
            fita.close();
        }
        for (auto& fita : fitas_saida_fase) {
            fita.close();
        }

        // verifica se a ordenação foi concluída
        ifstream primeira_fita(nomes_fitas[num_fitas / 2]);
        string conteudo;
        getline(primeira_fita, conteudo);
        primeira_fita.close();

        if (conteudo.find('\n') == string::npos) {  // se não houver novo bloco, a ordenação está finalizada
            ordenado = true;
            ifstream fita_final(nomes_fitas[num_fitas / 2]);
            ofstream arquivo_saida(nome_arquivo_saida);
            arquivo_saida << conteudo;
            fita_final.close();
            arquivo_saida.close();
        } else {
            // prepara para a próxima fase
            for (int i = 0; i < num_fitas / 2; ++i) {
                remove(nomes_fitas[i].c_str());
                rename(nomes_fitas[i + num_fitas / 2].c_str(), nomes_fitas[i].c_str());
            }
        }

        fase++;
    }

    // limpa as fitas temporárias
    for (const auto& nome_fita : nomes_fitas) {
        remove(nome_fita.c_str());
    }
}

int main() {
    UINT CPAGE_UTF8 = 65001;
    UINT CPAGE_DEFAULT = GetConsoleOutputCP();
    SetConsoleOutputCP(CPAGE_UTF8);

    string nome_arquivo_entrada = "entrada.txt";
    string nome_arquivo_saida = "saida.txt";
    int memoria_interna, num_fitas;

    // lê os parâmetros do arquivo de entrada
    ifstream arquivo_entrada(nome_arquivo_entrada);
    arquivo_entrada >> memoria_interna >> num_fitas;
    arquivo_entrada.ignore();  // ignora a nova linha após os números

    ordenacao_externa(nome_arquivo_entrada, nome_arquivo_saida, memoria_interna, num_fitas);

    cout << "Ordenação concluída. Resultado no arquivo " << nome_arquivo_saida << endl;

    return 0;
}

