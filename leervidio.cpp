#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <numeric> // para std::accumulate

using namespace std;
using namespace cv;

double calcularBrilloPromedio(const Mat &frame) {
    Mat gray;
    cvtColor(frame, gray, COLOR_BGR2GRAY);
    return mean(gray)[0];
}

double compararHistogramas(const Mat &frame1, const Mat &frame2) {
    Mat hsv1, hsv2;
    cvtColor(frame1, hsv1, COLOR_BGR2HSV);
    cvtColor(frame2, hsv2, COLOR_BGR2HSV);

    int hBins = 50, sBins = 60;
    int histSize[] = {hBins, sBins};
    float hRange[] = {0, 180};
    float sRange[] = {0, 256};
    const float *ranges[] = {hRange, sRange};
    int channels[] = {0, 1};

    Mat hist1, hist2;
    calcHist(&hsv1, 1, channels, Mat(), hist1, 2, histSize, ranges, true, false);
    normalize(hist1, hist1, 0, 1, NORM_MINMAX);

    calcHist(&hsv2, 1, channels, Mat(), hist2, 2, histSize, ranges, true, false);
    normalize(hist2, hist2, 0, 1, NORM_MINMAX);

    return compareHist(hist1, hist2, HISTCMP_CORREL);
}

int main() {
    VideoCapture cap("video.mp4");
    if (!cap.isOpened()) {
        cerr << "Error al abrir el video" << endl;
        return -1;
    }

    double fpsOriginal = cap.get(CAP_PROP_FPS);
    double fpsObjetivo = 4.0;
    int salto = static_cast<int>(fpsOriginal / fpsObjetivo);

    Mat frame, prevFrame;
    int frameCount = 0, cambiosEscena = 0, frameIndex = 0;
    vector<double> brillos, correlaciones;

    while (true) {
        cap.set(CAP_PROP_POS_FRAMES, frameIndex);
        if (!cap.read(frame))
            break;

        cap >> frame;
        if (frame.empty())
            break;

        if (!prevFrame.empty()) {
            double correlacion = compararHistogramas(prevFrame, frame);
            correlaciones.push_back(correlacion);
            if (correlacion < 0.8) {
                cambiosEscena++;
            }
        }

        double brillo = calcularBrilloPromedio(frame);
        brillos.push_back(brillo);

        prevFrame = frame.clone();
        frameCount++;

        if (waitKey(1) == 'q')
            break;

        prevFrame = frame.clone();
        frameIndex += salto;
        frameCount++;
    }

    double promedioBrillo = accumulate(brillos.begin(), brillos.end(), 0.0) / brillos.size();
    double varianzaBrillo = 0.0;
    for (double b : brillos)
        varianzaBrillo += (b - promedioBrillo) * (b - promedioBrillo);
    varianzaBrillo /= brillos.size();

    cout << "Frames ANALizados: " << frameCount << endl;
    cout << "Cambios de posicion sexual (correlación < 0.8): " << cambiosEscena << endl;
    cout << "Cantidad de cum (varianza de brillo): " << varianzaBrillo << endl;

    double fps = cap.get(CAP_PROP_FPS);
    double duracionSeg = frameCount / fps;
    double cambiosPorSegundo = cambiosEscena / duracionSeg;

    cout << "Cambios por segundo: " << cambiosPorSegundo << endl;

    bool esHiper = false;

    if (cambiosPorSegundo > 1.5) {
        cout << "El vidio tiene muchos cambios de escena" << endl;
        esHiper = true;
    }

    if (varianzaBrillo > 500) {
        cout << "El vidio tiene muchos cambios de brillo" << endl;
        esHiper = true;
    }

    if (esHiper)
        cout << "\nEal vidio es hipoerestimulante pq mariano siente riki" << endl;
    else
        cout << "\nMariano esta triste por que el video parece que no lo estimula suficiente" << endl;

    cap.release();
    return 0;
}
