#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#define MAX_RELATOS 1000
#define RAIO_MAXIMO 10.0 // 10 km

// Estrutura para armazenar coordenadas geográficas
typedef struct {
    double latitude;
    double longitude;
} Localizacao;

// Estrutura para armazenar dados pessoais
typedef struct {
    char nome_completo[100];
    char documento[20];
    char email[100];
    char telefone[20];
    Localizacao localizacao;
} DadosPessoais;

// Tipos de catástrofes
typedef enum {
    ENCHENTE,
    INCENDIO,
    DESLIZAMENTO,
    TERREMOTO,
    TSUNAMI,
    OUTRO
} TipoCatastrofe;

// Estrutura para armazenar um relato
typedef struct {
    DadosPessoais relator;
    TipoCatastrofe tipo;
    char descricao[500];
    time_t data_hora;
    Localizacao localizacao;
} Relato;

// Estrutura principal da aplicação
typedef struct {
    Relato relatos[MAX_RELATOS];
    int total_relatos;
    Localizacao ponto_central;
} SistemaRelatos;

// Função para calcular distância entre duas coordenadas geográficas (Haversine)
double calcular_distancia(Localizacao loc1, Localizacao loc2) {
    double lat1 = loc1.latitude * M_PI / 180.0;
    double lon1 = loc1.longitude * M_PI / 180.0;
    double lat2 = loc2.latitude * M_PI / 180.0;
    double lon2 = loc2.longitude * M_PI / 180.0;
    
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;
    
    double a = sin(dlat / 2) * sin(dlat / 2) + 
               cos(lat1) * cos(lat2) * 
               sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    
    // Raio da Terra em km
    double raio_terra = 6371.0;
    
    return raio_terra * c;
}

// Função para validar se um relato está dentro do raio permitido
int validar_localizacao(Localizacao ponto_central, Localizacao local_relato) {
    double distancia = calcular_distancia(ponto_central, local_relato);
    return distancia <= RAIO_MAXIMO;
}

// Função para converter string para TipoCatastrofe
TipoCatastrofe string_para_tipo(const char* tipo_str) {
    if (strcasecmp(tipo_str, "enchente") == 0) return ENCHENTE;
    if (strcasecmp(tipo_str, "incendio") == 0) return INCENDIO;
    if (strcasecmp(tipo_str, "deslizamento") == 0) return DESLIZAMENTO;
    if (strcasecmp(tipo_str, "terremoto") == 0) return TERREMOTO;
    if (strcasecmp(tipo_str, "tsunami") == 0) return TSUNAMI;
    return OUTRO;
}

// Função para converter TipoCatastrofe para string
const char* tipo_para_string(TipoCatastrofe tipo) {
    switch(tipo) {
        case ENCHENTE: return "Enchente";
        case INCENDIO: return "Incêndio";
        case DESLIZAMENTO: return "Deslizamento";
        case TERREMOTO: return "Terremoto";
        case TSUNAMI: return "Tsunami";
        default: return "Outro";
    }
}

// Função para formatar data e hora
void formatar_data_hora(time_t data_hora, char* buffer, int tamanho) {
    struct tm *tm_info = localtime(&data_hora);
    strftime(buffer, tamanho, "%d/%m/%Y %H:%M", tm_info);
}

// Função para inicializar o sistema
void inicializar_sistema(SistemaRelatos *sistema, double lat_central, double lon_central) {
    sistema->total_relatos = 0;
    sistema->ponto_central.latitude = lat_central;
    sistema->ponto_central.longitude = lon_central;
}

// Função para cadastrar um novo relato
int cadastrar_relato(SistemaRelatos *sistema) {
    if (sistema->total_relatos >= MAX_RELATOS) {
        printf("Limite máximo de relatos atingido!\n");
        return 0;
    }
    
    Relato novo_relato;
    
    // Dados do relator
    printf("\n=== Dados do Relator ===\n");
    printf("Nome completo: ");
    fgets(novo_relato.relator.nome_completo, sizeof(novo_relato.relator.nome_completo), stdin);
    novo_relato.relator.nome_completo[strcspn(novo_relato.relator.nome_completo, "\n")] = '\0';
    
    printf("Documento (CPF/RG): ");
    fgets(novo_relato.relator.documento, sizeof(novo_relato.relator.documento), stdin);
    novo_relato.relator.documento[strcspn(novo_relato.relator.documento, "\n")] = '\0';
    
    printf("E-mail: ");
    fgets(novo_relato.relator.email, sizeof(novo_relato.relator.email), stdin);
    novo_relato.relator.email[strcspn(novo_relato.relator.email, "\n")] = '\0';
    
    printf("Telefone: ");
    fgets(novo_relato.relator.telefone, sizeof(novo_relato.relator.telefone), stdin);
    novo_relato.relator.telefone[strcspn(novo_relato.relator.telefone, "\n")] = '\0';
    
    printf("Localização do relator (latitude longitude): ");
    scanf("%lf %lf", &novo_relato.relator.localizacao.latitude, &novo_relato.relator.localizacao.longitude);
    while(getchar() != '\n'); // Limpar buffer
    
    // Dados do relato
    printf("\n=== Dados do Relato ===\n");
    
    char tipo_str[50];
    printf("Tipo da catástrofe (enchente, incendio, deslizamento, terremoto, tsunami, outro): ");
    fgets(tipo_str, sizeof(tipo_str), stdin);
    tipo_str[strcspn(tipo_str, "\n")] = '\0';
    novo_relato.tipo = string_para_tipo(tipo_str);
    
    printf("Descrição: ");
    fgets(novo_relato.descricao, sizeof(novo_relato.descricao), stdin);
    novo_relato.descricao[strcspn(novo_relato.descricao, "\n")] = '\0';
    
    // Data e hora atual
    novo_relato.data_hora = time(NULL);
    
    printf("Localização da catástrofe (latitude longitude): ");
    scanf("%lf %lf", &novo_relato.localizacao.latitude, &novo_relato.localizacao.longitude);
    while(getchar() != '\n'); // Limpar buffer
    
    // Validar localização
    if (!validar_localizacao(sistema->ponto_central, novo_relato.localizacao)) {
        printf("Erro: O local do relato está fora do raio de 10 km do ponto central.\n");
        return 0;
    }
    
    // Adicionar relato ao sistema
    sistema->relatos[sistema->total_relatos] = novo_relato;
    sistema->total_relatos++;
    
    printf("Relato cadastrado com sucesso!\n");
    return 1;
}

// Função para listar todos os relatos
void listar_relatos(SistemaRelatos *sistema) {
    printf("\n=== Relatos Cadastrados ===\n");
    printf("Total: %d\n\n", sistema->total_relatos);
    
    for (int i = 0; i < sistema->total_relatos; i++) {
        Relato r = sistema->relatos[i];
        char data_hora_str[20];
        formatar_data_hora(r.data_hora, data_hora_str, sizeof(data_hora_str));
        
        printf("Relato #%d\n", i+1);
        printf("Tipo: %s\n", tipo_para_string(r.tipo));
        printf("Data/Hora: %s\n", data_hora_str);
        printf("Localização: %.6f, %.6f\n", r.localizacao.latitude, r.localizacao.longitude);
        printf("Descrição: %s\n", r.descricao);
        printf("Relator: %s (%s)\n", r.relator.nome_completo, r.relator.documento);
        printf("Contato: %s, %s\n", r.relator.email, r.relator.telefone);
        printf("----------------------------\n");
    }
}

// Função para buscar relatos por tipo
void buscar_por_tipo(SistemaRelatos *sistema) {
    char tipo_str[50];
    printf("Digite o tipo de catástrofe a buscar: ");
    fgets(tipo_str, sizeof(tipo_str), stdin);
    tipo_str[strcspn(tipo_str, "\n")] = '\0';
    
    TipoCatastrofe tipo = string_para_tipo(tipo_str);
    
    printf("\n=== Relatos de %s ===\n", tipo_para_string(tipo));
    
    int encontrados = 0;
    for (int i = 0; i < sistema->total_relatos; i++) {
        if (sistema->relatos[i].tipo == tipo) {
            Relato r = sistema->relatos[i];
            char data_hora_str[20];
            formatar_data_hora(r.data_hora, data_hora_str, sizeof(data_hora_str));
            
            printf("Relato #%d\n", i+1);
            printf("Data/Hora: %s\n", data_hora_str);
            printf("Localização: %.6f, %.6f\n", r.localizacao.latitude, r.localizacao.longitude);
            printf("Descrição: %s\n", r.descricao);
            printf("Relator: %s\n", r.relator.nome_completo);
            printf("----------------------------\n");
            encontrados++;
        }
    }
    
    if (encontrados == 0) {
        printf("Nenhum relato encontrado para este tipo.\n");
    } else {
        printf("Total encontrado: %d\n", encontrados);
    }
}

// Função para buscar relatos por período
// Função para buscar relatos por período (versão compatível com Windows)
void buscar_por_periodo(SistemaRelatos *sistema) {
    struct tm tm_inicio = {0};
    struct tm tm_fim = {0};
    char buffer[20];
    int dia, mes, ano;
    
    printf("Data de início (DD/MM/AAAA): ");
    fgets(buffer, sizeof(buffer), stdin);
    if (sscanf(buffer, "%d/%d/%d", &dia, &mes, &ano) != 3) {
        printf("Formato de data inválido!\n");
        return;
    }
    tm_inicio.tm_mday = dia;
    tm_inicio.tm_mon = mes - 1;
    tm_inicio.tm_year = ano - 1900;
    
    printf("Data de fim (DD/MM/AAAA): ");
    fgets(buffer, sizeof(buffer), stdin);
    if (sscanf(buffer, "%d/%d/%d", &dia, &mes, &ano) != 3) {
        printf("Formato de data inválido!\n");
        return;
    }
    tm_fim.tm_mday = dia;
    tm_fim.tm_mon = mes - 1;
    tm_fim.tm_year = ano - 1900;
    
    time_t inicio = mktime(&tm_inicio);
    time_t fim = mktime(&tm_fim);
    
    printf("\n=== Relatos entre %02d/%02d/%04d e %02d/%02d/%04d ===\n",
           tm_inicio.tm_mday, tm_inicio.tm_mon + 1, tm_inicio.tm_year + 1900,
           tm_fim.tm_mday, tm_fim.tm_mon + 1, tm_fim.tm_year + 1900);
    
    int encontrados = 0;
    for (int i = 0; i < sistema->total_relatos; i++) {
        if (sistema->relatos[i].data_hora >= inicio && sistema->relatos[i].data_hora <= fim) {
            Relato r = sistema->relatos[i];
            char data_hora_str[20];
            formatar_data_hora(r.data_hora, data_hora_str, sizeof(data_hora_str));
            
            printf("Relato #%d\n", i+1);
            printf("Tipo: %s\n", tipo_para_string(r.tipo));
            printf("Data/Hora: %s\n", data_hora_str);
            printf("Localização: %.6f, %.6f\n", r.localizacao.latitude, r.localizacao.longitude);
            printf("Descrição: %s\n", r.descricao);
            printf("Relator: %s\n", r.relator.nome_completo);
            printf("----------------------------\n");
            encontrados++;
        }
    }
    
    if (encontrados == 0) {
        printf("Nenhum relato encontrado neste período.\n");
    } else {
        printf("Total encontrado: %d\n", encontrados);
    }
}

// Função para buscar relatos por localização (aproximada)
void buscar_por_localizacao(SistemaRelatos *sistema) {
    Localizacao loc;
    double distancia_maxima;
    
    printf("Localização de referência (latitude longitude): ");
    scanf("%lf %lf", &loc.latitude, &loc.longitude);
    while(getchar() != '\n'); // Limpar buffer
    
    printf("Distância máxima em km: ");
    scanf("%lf", &distancia_maxima);
    while(getchar() != '\n'); // Limpar buffer
    
    printf("\n=== Relatos próximos a %.6f, %.6f (%.1f km) ===\n", 
           loc.latitude, loc.longitude, distancia_maxima);
    
    int encontrados = 0;
    for (int i = 0; i < sistema->total_relatos; i++) {
        double distancia = calcular_distancia(loc, sistema->relatos[i].localizacao);
        if (distancia <= distancia_maxima) {
            Relato r = sistema->relatos[i];
            char data_hora_str[20];
            formatar_data_hora(r.data_hora, data_hora_str, sizeof(data_hora_str));
            
            printf("Relato #%d (%.1f km)\n", i+1, distancia);
            printf("Tipo: %s\n", tipo_para_string(r.tipo));
            printf("Data/Hora: %s\n", data_hora_str);
            printf("Localização: %.6f, %.6f\n", r.localizacao.latitude, r.localizacao.longitude);
            printf("Descrição: %s\n", r.descricao);
            printf("Relator: %s\n", r.relator.nome_completo);
            printf("----------------------------\n");
            encontrados++;
        }
    }
    
    if (encontrados == 0) {
        printf("Nenhum relato encontrado nesta área.\n");
    } else {
        printf("Total encontrado: %d\n", encontrados);
    }
}

// Função para exibir o menu principal
void exibir_menu() {
    printf("\n=== Sistema de Relatos de Catástrofes Naturais ===\n");
    printf("1. Cadastrar novo relato\n");
    printf("2. Listar todos os relatos\n");
    printf("3. Buscar relatos por tipo\n");
    printf("4. Buscar relatos por período\n");
    printf("5. Buscar relatos por localização\n");
    printf("6. Sair\n");
    printf("Escolha uma opção: ");
}

// Função principal
int main() {
    SistemaRelatos sistema;
    
    // Configurar ponto central (exemplo: centro de São Paulo)
    inicializar_sistema(&sistema, -23.5505, -46.6333);
    
    int opcao;
    do {
        exibir_menu();
        scanf("%d", &opcao);
        while(getchar() != '\n'); // Limpar buffer
        
        switch(opcao) {
            case 1:
                cadastrar_relato(&sistema);
                break;
            case 2:
                listar_relatos(&sistema);
                break;
            case 3:
                buscar_por_tipo(&sistema);
                break;
            case 4:
                buscar_por_periodo(&sistema);
                break;
            case 5:
                buscar_por_localizacao(&sistema);
                break;
            case 6:
                printf("Saindo do sistema...\n");
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while (opcao != 6);
    
    return 0;
}