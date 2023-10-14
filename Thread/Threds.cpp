#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <string>
#include <iostream>
#include <pthread.h>

// Estrutura para passar argumentos para a thread
typedef struct {
    cv::Mat imagem;
    cv::Mat Gx;
    cv::Mat Gy;
} ThreadArgs;

// Função para calcular a borda na direção x
void* calcular_borda_x(void* arguments) {
    ThreadArgs* args = (ThreadArgs*)arguments;
    cv::Mat imagem = args->imagem;
    cv::Mat Gx = args->Gx;

    for (int i = 1; i < imagem.rows - 1; ++i) {
        for (int j = 1; j < imagem.cols - 1; ++j) {
            Gx.at<uchar>(i, j) = abs(imagem.at<uchar>(i + 1, j - 1) + 2 * imagem.at<uchar>(i + 1, j) +
                imagem.at<uchar>(i + 1, j + 1) - imagem.at<uchar>(i - 1, j - 1) -
                2 * imagem.at<uchar>(i - 1, j) - imagem.at<uchar>(i - 1, j + 1));
            Gx.at<uchar>(i, j) = std::min(255, std::max(0, (int)Gx.at<uchar>(i, j)));
        }
    }

    pthread_exit(NULL);
}

// Função para calcular a borda na direção y
void* calcular_borda_y(void* arguments) {
    ThreadArgs* args = (ThreadArgs*)arguments;
    cv::Mat imagem = args->imagem;
    cv::Mat Gy = args->Gy;

    for (int i = 1; i < imagem.rows - 1; ++i) {
        for (int j = 1; j < imagem.cols - 1; ++j) {
            Gy.at<uchar>(i, j) = abs(imagem.at<uchar>(i - 1, j + 1) + 2 * imagem.at<uchar>(i, j + 1) +
                imagem.at<uchar>(i + 1, j + 1) - imagem.at<uchar>(i - 1, j - 1) -
                2 * imagem.at<uchar>(i, j - 1) - imagem.at<uchar>(i + 1, j - 1));
            Gy.at<uchar>(i, j) = std::min(255, std::max(0, (int)Gy.at<uchar>(i, j)));
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    char diretorio[500]; 

    std::cout << "Digite o endereço da imagem: " << std::endl;
    std::cin >> diretorio;

    cv::Mat imagem_cinza = cv::imread(diretorio, cv::IMREAD_GRAYSCALE);

    if (imagem_cinza.empty()) {
        printf("Não foi possível ler a imagem\n");
        return -1;
    }

    cv::Mat Gx = cv::Mat::zeros(imagem_cinza.size(), CV_8U);
    cv::Mat Gy = cv::Mat::zeros(imagem_cinza.size(), CV_8U);

    // Estrutura para passar argumentos para as threads
    ThreadArgs thread_args = { imagem_cinza, Gx, Gy };

    // Inicializar as threads
    pthread_t thread_x, thread_y;

    // Calcular as bordas nas direções x e y
    pthread_create(&thread_x, NULL, calcular_borda_x, (void*)&thread_args);
    pthread_create(&thread_y, NULL, calcular_borda_y, (void*)&thread_args);

    // Aguardar o término das threads
    pthread_join(thread_x, NULL);
    pthread_join(thread_y, NULL);

    // Calcular a imagem de saída G
    cv::Mat imagem_saida;
    cv::add(Gx, Gy, imagem_saida);

    // Exibir a imagem de saída
    cv::imshow("Imagem de Saída", imagem_saida);
    cv::imwrite("ImagemSaida.png", imagem_saida);
    cv::waitKey(0);

    return 0;
}
