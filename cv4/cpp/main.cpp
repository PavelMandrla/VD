#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <stdio.h>
#include <algorithm>
#include <cmath>
#include <fstream>

using namespace std;
using namespace cv;

vector<Mat> loadMats() {
    vector<Mat> result;
    for ( int i = 0; i < 1000; ++i ) {
        char file_name[128];
        sprintf(file_name, "../../data/u%05d.yml", i);
        cv::Mat u;
        cv::FileStorage fs(file_name, cv::FileStorage::READ);
        fs["flow"] >> u;

        result.push_back(u);
    }
    return result;
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
        Vec3b color;
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

            this->values.push_back(ValCol { value, Vec3b(b, g, r)});
        }
    }

    Vec3b getColor(float value) {   // OPTIMALIZACE PRICHAZEJI POZDEJI
        for (int i = 0; i < this->values.size() - 1; i++) {
            if (this->values[i + 1].value > value) {
                return this->values[i].color;
            }
        }
        return this->values[this->values.size() - 1].color;
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
    Mat result = Mat::zeros(input.size(), input.type());

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
    Mat result = Mat::zeros(input.size(), CV_32FC3);
    vector<Mat> channels(2);
    split(input, channels);

    auto vx_dy = getDerivatives(channels[0], dy);
    auto vy_dx = getDerivatives(channels[1], dx);

    for (int y = 0; y < result.rows; y++) {
        for (int x = 0; x < result.cols; x++) {
            float val = vy_dx.at<float>(y, x) - vx_dy.at<float>(y, x);
            result.at<Vec3f>(y, x) = Vec3f(0, 0, (val + 1.0) / 2.0);
        }
    }
    return result;
}

Mat getVelocities(Mat input) {
    Mat result = Mat::zeros(input.size(), CV_32FC1);
    for (int y = 0; y < result.rows; y++) {
        for (int x = 0; x < result.cols; x++) {
            result.at<float>(y, x) = sqrt(pow(input.at<Vec2f>(y, x)[0],2) + pow(input.at<Vec2f>(y, x)[1],2)) / sqrt(2.0);
        }
    }
    return result;
}


int main() {
    auto map = ColorMap("../../plasma-table-byte-1024.csv");
    auto mats = loadMats();

    vector<Particle*> particles;

    for (auto mat : mats) {
        auto divergence = getDivergence(mat);
        auto curl = getCurl(mat);
        auto velocities = getVelocities(mat);

        particles.push_back(new Particle(120.0, 200.0, 1.0));



        Mat velocities_col = Mat::zeros(velocities.size(), CV_8UC3);
        Mat curl_col = Mat::zeros(curl.size(), CV_8UC3);
        Mat divergence_col = Mat::zeros(curl.size(), CV_8UC3);
        for (int y = 0; y < velocities_col.rows; y++) {
            for (int x = 0; x < velocities_col.cols; x++) {
                velocities_col.at<Vec3b>(y, x) = map.getColor(velocities.at<float>(y, x));
                curl_col.at<Vec3b>(y, x) = map.getColor(curl.at<Vec3f>(y, x)[2]);
                divergence_col.at<Vec3b>(y, x) = map.getColor(divergence.at<float>(y, x));
            }
        }

        for (int x = 0; x < mats[0].cols; x+=20) {
            for (int y = 0; y < mats[0].rows; y+=20) {
                Point p1(x, y);
                auto vec = mat.at<Vec2f>(y, x) * 3;
                Point p2(x + vec[0], y + vec[1]);
                arrowedLine(velocities_col, p1, p2, Vec3b(255, 255, 255));
            }
        }

        //for (auto p : particles) {
        for (int i = 0; i < particles.size(); i++) {
            auto p = particles[i];
            p->move_RK4(mat);
            int x = std::clamp(int(p->getX()), 0, velocities_col.cols-1);
            int y = std::clamp(int(p->getY()), 0, velocities_col.rows-1);

            if (i > 0) {
                auto p1 = particles[i-1];

                Point p_1(p->getX(), p->getY());
                Point p_2(p1->getX(), p1->getY());
                line(velocities_col, p_1, p_2, Vec3b(0, 0, 0), 2);
                line(velocities_col, p_1, p_2, Vec3b(0, 255, 0));

            }

            velocities_col.at<Vec3b>(y, x) = Vec3b(0, 255, 0);
            curl_col.at<Vec3b>(y, x) = Vec3b(0, 255, 0);
            divergence_col.at<Vec3b>(y, x) = Vec3b(0, 255, 0);
        }


        //imshow("divergence", divergence);
        imshow("divergence colorized", divergence_col);
        //imshow("curl", curl);
        imshow("curl colorized", curl_col);
        imshow("velocities", velocities);
        imshow("velocities colorized", velocities_col);

        waitKey(20);
    }

    return 0;
}
