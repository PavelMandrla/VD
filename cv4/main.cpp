#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <fstream>

using namespace std;
using namespace cv;

Mat loadMat(int i) {
    char file_name[128];
    sprintf(file_name, "../data/u%05d.yml", i);
    cv::Mat u;
    cv::FileStorage fs(file_name, cv::FileStorage::READ);
    fs["flow"] >> u;
    return u;
}

enum DWhat {dx, dy};

Vec2f getVecFieldVal(Vec2f pos, Mat vecField)  {
    // TODO -> interpolate values??
    int x = std::clamp(int(pos[0]), 0, vecField.cols-1);
    int y = std::clamp(int(pos[1]), 0, vecField.rows-1);
    return vecField.at<Vec2f>(y, x);
}

class ColorMap {
private:
    struct ValCol {
        float value;
        Vec4b color;
    };
    vector<ValCol> values;

public:
    ColorMap(string path) {
        ifstream inFile(path);
        string line;
        getline(inFile, line);
        while (getline(inFile, line)) {
            float value;
            int r, g, b;
            istringstream iss(line);
            string tmp;
            getline(iss, tmp, ',');
            value = stof(tmp);
            getline(iss, tmp, ',');
            r = stoi(tmp);
            getline(iss, tmp, ',');
            g = stoi(tmp);
            getline(iss, tmp, ',');
            b = stoi(tmp);

            this->values.push_back(ValCol { value, Vec4b(b, g, r, 255)});
        }
    }

    Vec4b getColor(float value) {   // OPTIMALIZACE PRICHAZEJI POZDEJI
        for (int i = 0; i < this->values.size() - 1; i++) {
            if (this->values[i + 1].value > value) {
                return this->values[i].color;
            }
        }
        return this->values[this->values.size() - 1].color;
    }

    Mat colorize(const Mat& input) {
        Mat output = Mat::zeros(input.rows, input.cols, CV_8UC4);
        for (int y = 0; y < input.rows; y++) {
            for (int x = 0; x < input.cols; x++) {
                output.at<Vec4b>(y, x) = this->getColor(input.at<float>(y, x));
            }
        }
        return output;
    }
};

class Particle {
private:
    Vec2f pos;
    float dt = 1;

    Vec2f K1(Mat vfield) {
        return getVecFieldVal(this->pos, vfield) * this->dt;
    }

    Vec2f K2(Mat vfield, const Vec2f& K1) {
        Vec2f nPos = this->pos + K1 / 2;
        return getVecFieldVal(nPos, vfield) * this->dt;
    }

    Vec2f K3(Mat vfield, const Vec2f& K2) {
        Vec2f nPos = this->pos + K2 / 2;
        return getVecFieldVal(nPos, vfield) * this->dt;
    }

    Vec2f K4(Mat vfield, const Vec2f& K3) {
        Vec2f nPos = this->pos + K3;
        return getVecFieldVal(nPos, vfield) * this->dt;
    }
public:
    Particle(float x, float y, float dt = 1.0) {
        this->dt = dt;
        this->pos = Vec2f(x, y);
    }

    void move_RK4(const Mat& vfield) {
        auto k1 = this->K1(vfield);
        auto k2 = this->K2(vfield, k1);
        auto k3 = this->K3(vfield, k2);
        auto k4 = this->K4(vfield, k3);

        this->pos += (k1 + 2*k2 + 2*k3 + k3)/6.0;
    }

    Vec2f getPos() {
        return this->pos;
    }

    int getX() {
        return int(this->pos[0]);
    }

    int getY() {
        return int(this->pos[1]);
    }
};


Mat getDerivatives(Mat input, DWhat dWhat) {
    Mat result = Mat::zeros(input.rows, input.cols, input.type());

    for (int y = 2; y < input.rows - 2; y++) {
        for (int x = 2; x < input.cols - 2; x++) {
            float h = 1.0;
            float f_m2 = dWhat == dx ? input.at<float>(y, x-2) : input.at<float>(y-2, x);
            float f_m1= dWhat == dx ? input.at<float>(y, x-1) : input.at<float>(y-1, x);
            float f_1 = dWhat == dx ? input.at<float>(y, x+1) : input.at<float>(y+2, x);
            float f_2 = dWhat == dx ? input.at<float>(y, x+2) : input.at<float>(y+2, x);

            result.at<float>(y, x) = (-f_2 + 8 * f_1 - 8 * f_m1 + f_m2) / (12*h);
        }
    }

    return result;
}

Mat getDivergence(Mat input) {
    vector<Mat> channels(2);
    split(input, channels);

    auto vx_dx = getDerivatives(channels[0], dx);
    auto vy_dy = getDerivatives(channels[1], dy);

    for (int y = 0; y < vx_dx.rows; y++) {
        for (int x = 0; x < vx_dx.cols; x++) {
            vx_dx.at<float>(y, x) += vy_dy.at<float>(y, x);
        }
    }
    return vx_dx;
}

Mat getCurl(Mat input) {
    Mat result = Mat::zeros(input.rows, input.cols, CV_32FC1);
    vector<Mat> channels(2);
    split(input, channels);

    auto vx_dy = getDerivatives(channels[0], dy);
    auto vy_dx = getDerivatives(channels[1], dx);

    for (int y = 0; y < result.rows; y++) {
        for (int x = 0; x < result.cols; x++) {
            float val = vy_dx.at<float>(y, x) - vx_dy.at<float>(y, x);
            result.at<float>(y, x) =(val + 1.0) / 2.0;
        }
    }
    return result;
}

Mat getVelocities(Mat input) {
    Mat result = Mat::zeros(input.rows, input.cols, CV_32FC1);
    for (int y = 0; y < result.rows; y++) {
        for (int x = 0; x < result.cols; x++) {
            result.at<float>(y, x) = sqrt(pow(input.at<Vec2f>(y, x)[0],2) + pow(input.at<Vec2f>(y, x)[1],2)) / sqrt(2.0);
        }
    }
    return result;
}

void drawLines(Mat &mat_col, Mat &mat_src) {
    srand(0);
    for (int x = 15; x < mat_src.cols; x += 15) {
        for (int y = 15; y < mat_src.rows; y += 15) {
            auto nx = x + int((rand() % 10)  - 10);
            auto ny = y + int((rand() % 10)  - 10);
            auto vec = mat_src.at<Vec2f>(ny, nx) * 3;
            Point p1(nx, ny);
            Point p2(nx + vec[0], ny + vec[1]);
            if (norm(p1 - p2) > 1.2) {
                arrowedLine(mat_col, p1, p2, Vec4b(0, 0, 0, 0));
            }
        }
    }
}


int main() {
    auto map_plasma = ColorMap("../plasma-table-byte-1024.csv");
    auto map_warm = ColorMap("../smooth-cool-warm-table-byte-1024.csv");
    namedWindow("curl", WINDOW_NORMAL);
    namedWindow("velocities", WINDOW_NORMAL);

    vector<Particle*> particles;

    for (int i = 0; i < 1000; i++) {
        auto mat = loadMat(i);

        const auto divergence = getDivergence(mat);
        const auto curl = getCurl(mat);
        const auto velocities = getVelocities(mat);

        particles.push_back(new Particle(120.0, 200.0, 1.0));

        Mat velocities_col = map_plasma.colorize(velocities);
        Mat curl_col = map_warm.colorize(curl);
        Mat divergence_col = map_plasma.colorize(divergence);

        drawLines(curl_col, mat);
        drawLines(velocities_col, mat);

        for (int j = 0; j < particles.size(); j++) {
            auto p = particles[j];
            p->move_RK4(mat);

            if (j > 0) {
                auto p1 = particles[j-1];
                Point p_1(p->getX(), p->getY());
                Point p_2(p1->getX(), p1->getY());

                line(curl_col, p_1, p_2, Vec3b(70, 70, 255));
                line(velocities_col, p_1, p_2, Vec3b(70, 70, 255));
            }
        }

        imshow("curl", curl_col);
        imshow("velocities", velocities_col);

        waitKey(20);
    }
    return 0;
}
