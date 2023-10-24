// final_CPO_Projekt2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

using namespace std;
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>


string img_path = "C:/Users/Dom/Desktop/Dominik_G/1.png";

//Wyznaczone momentyhu do rozpoznania karty dla każdej z danych cyfr i symbolu
const double hu_min_7 = 0.95;
const double hu_max_7 = 1.55;

const double hu_min_9 = 3.0;
const double hu_max_9 = 4.1;
const double hu_min1_9 = 1.8;
const double hu_max1_9 = 3.4;

const double hu_min_3 = 2.2;
const double hu_max_3 = 2.95;
const double hu_min6_3 = -6.49;
const double hu_max6_3 = -4.5;

const double hu_min_stop = 6.3;
const double hu_max_stop = 8.8;

const double hu_min_arrow = 2.1;
const double hu_max_arrow = 5.4;

const double hu_min1_arrow = 0.75;
const double hu_max1_arrow = 1.5;

const double hu_min3_arrow = 2.1;
const double hu_max3_arrow = 3.3;

const double hu_bot6_arrow = -6.5;
const double hu_top6_arrow = 5;





void WgranieObrazu(cv::Mat& img)
{


    img = cv::imread(img_path, cv::IMREAD_COLOR);

    if (img.empty())
    {
        cout << "Nie mozna znalezc obrazu\n" << endl;

    }
   
    resize(img, img, { 500, 500 }, 0, 0, cv::INTER_NEAREST);

}

void pre_processing(cv::Mat& img, cv::Mat& threshed_shapes, cv::Mat& shapes)
{

    cv::Mat diff, unsharped, gaussian_blur;

    //cv::GaussianBlur(img, gaussian_blur, cv::Size(17, 17), 15.0);                          
    //cv::addWeighted(img, 2.0, gaussian_blur, -1.0, 0, unsharped);                   

    cv::Mat filtered;
    cv::medianBlur(img, filtered, 3);                           // medianBlur użyty w celu pozbycia się zakłóceń salt_pepper jak i blur



    img = filtered;


    shapes = img;
    cv::Mat gray_shapes ;
    cv::cvtColor(shapes, gray_shapes, cv::COLOR_BGR2GRAY);                           //zmiana na jedno kanalowy grayscale aby pozniej zrobic threshold
    cv::threshold(gray_shapes, threshed_shapes, 198, 255, cv::THRESH_BINARY);

}


void kontury(cv::Mat& threshed_shapes, cv::Mat& shapes, std::vector<std::vector<cv::Point>>& contours, std::vector<cv::Vec4i>& hierarchy )
{

    

    cv::findContours(threshed_shapes, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);   //stosujemy tutaj RETR_EXTERNAL bo zalezy nam tylko na odzieleniu kart od tła
    //cv::drawContours(shapes, contours, -1, cv::Scalar(0, 0, 255), 3);

}

void detekcja_kolorow(cv::Mat img, cv::Mat& blue_threshold, cv::Mat& red_threshold, cv::Mat& green_threshold, cv::Mat& yellow_threshold)
{

    cv::Mat img_hsv;
    cv::cvtColor(img, img_hsv, cv::COLOR_BGR2HSV);          // dzialamy na przestrzeni HSV jako ze jest lepsza jesli chodzi o odseparowanie kolorow

    cv::inRange(img_hsv, cv::Scalar(100, 230, 180), cv::Scalar(120, 255, 210), blue_threshold);

    cv::inRange(img_hsv, cv::Scalar(0, 210, 140), cv::Scalar(20, 255, 190), red_threshold);

    cv::inRange(img_hsv, cv::Scalar(30, 230, 140), cv::Scalar(60, 255, 170), green_threshold);

    cv::inRange(img_hsv, cv::Scalar(0, 210, 200), cv::Scalar(50, 253, 253), yellow_threshold);

}


int jaki_kolor_karta(cv::Mat blue_threshold, cv::Mat red_threshold, cv::Mat green_threshold, cv::Mat yellow_threshold,cv::Point center)
{
   int kolor = 0;
    std::vector<std::vector<cv::Point>> contours_blue,contours_red,contours_green,contours_yellow;

    cv::findContours(blue_threshold, contours_blue, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    //cv::drawContours(shapes, contours_blue, -1, cv::Scalar(0, 255, 0), 3);

    for (int j = 0; j < contours_blue.size(); j++)
    {
        double k = 0;
        k = cv::pointPolygonTest(contours_blue[j], center, false);                        //pointPolygonTest zwraca wartosc 1,-1,0 w zalezonsci czy punk znajduje sie w srodku na grnicy lub poza konturami
        if (k == 1)                                                                       
        {
            kolor=1;
            cout << "karta jest niebieska" << endl;
        }
     }

    cv::findContours(red_threshold, contours_red, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (int j = 0; j < contours_red.size(); j++)
    {
        double k = 0;
        k = cv::pointPolygonTest(contours_red[j], center, false);
        if (k == 1)
        {
            kolor=2;
            cout << "karta jest czerwona" << endl;
        }
    }

    cv::findContours(green_threshold, contours_green, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (int j = 0; j < contours_green.size(); j++)
    {
        double k = 0;
        k = cv::pointPolygonTest(contours_green[j], center, false);
        if (k == 1)
        {
            kolor=3;
            cout << "karta jest zielona" << endl;
        }
    }

    cv::findContours(yellow_threshold, contours_yellow, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (int j = 0; j < contours_yellow.size(); j++)
    {
        double k = 0;
        k = cv::pointPolygonTest(contours_yellow[j], center, false);
        if (k == 1)
        {
            kolor=4;
            cout << "karta jest zolta" << endl;
        }
    }
    return(kolor);
}




void detekcja_symbolu(cv::Mat& symbol, cv::Mat& shapes,  cv::RotatedRect box,double& suma, double& suma_b, double& suma_r, double& suma_g, double& suma_y, cv::Mat blue_threshold, cv::Mat red_threshold, cv::Mat green_threshold, cv::Mat yellow_threshold)
{


    std::vector<std::vector<cv::Point>> contours_symbol;
    std::vector<cv::Vec4i> hierarchy_symbol;
    int kolor = 0;

    cv::findContours(symbol, contours_symbol, hierarchy_symbol, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);             //wyznaczenie kontur na naszym ROI
    //cv::drawContours(shapes, contours_symbol, -1, cv::Scalar(0, 0, 255), 3);


    std::vector<cv::Point> maxcnt = contours_symbol[0];


    for (int i = 0; i < contours_symbol.size(); i++)              //znajdowanie największego konturu na obszarze symbol
    {
        if (maxcnt.size() < contours_symbol[i].size())
        {
            maxcnt = contours_symbol[i];
        }

    }

    
    auto moments = cv::moments(maxcnt);
    cv::Point2d center = cv::Point2d{ moments.m10 / moments.m00, moments.m01 / moments.m00 };
    double huMoments[7];
    cv::HuMoments(moments, huMoments);
    for (int m = 0; m < 7; m++)
    {
        huMoments[m] = -1 * copysign(1.0, huMoments[m]) * log10(abs(huMoments[m]));                      //łatwiejsza postać do analizy
    }


    


    if (huMoments[2] >= hu_min_7 && huMoments[2] <= hu_max_7)
    {
        cout << "jest to 7" << endl;
        cv::Point center = box.center;                                  // wyznaczenie srodka karty poprzez minAreaRect
        cout << "o wspolrzednych " << center << endl;


        kolor = jaki_kolor_karta(blue_threshold, red_threshold, green_threshold,yellow_threshold, center);

       // cout << kolor << endl;

        switch (kolor)
        {
        case 1:
            suma_b = suma_b + 7;
            break;
        case(2):
            suma_r = suma_r + 7;
            break;
        case(3):
            suma_g = suma_g + 7;
            break;
        case(4):
            suma_y = suma_y + 7;
            break;
        }
        
           
        suma = suma + 7;

    }

    if (huMoments[2] >= hu_min_9 && huMoments[2] <= hu_max_9 && huMoments[1] >= hu_min1_9 && huMoments[1] <= hu_max1_9)
    {
        cout << "jest to 9" << endl;
        cv::Point center = box.center;                                  // wyznaczenie srodka karty poprzez minAreaRect
        cout << "o wspolrzednych " << center << endl;

        kolor = jaki_kolor_karta(blue_threshold, red_threshold, green_threshold, yellow_threshold, center);
       // cout << kolor << endl;
        switch (kolor)
        {
        case 1:
            suma_b = suma_b + 9;
            break;
        case(2):
            suma_r = suma_r + 9;
            break;
        case(3):
            suma_g = suma_g + 9;
            break;
        case(4):
            suma_y = suma_y + 9;
            break;
        }
        suma = suma + 9;
    }

    if (huMoments[2] >= hu_min_stop && huMoments[2] <= hu_max_stop)
    {
        cout << "jest to stop" << endl;

        cv::Point center = box.center;                                  // wyznaczenie srodka karty poprzez minAreaRect
        cout << "o wspolrzednych " << center << endl;

        kolor = jaki_kolor_karta(blue_threshold, red_threshold, green_threshold, yellow_threshold, center);
        //cout << kolor << endl;
    }

    if (huMoments[2] >= hu_min_3 && huMoments[2] <= hu_max_3 && huMoments[6] >= hu_min6_3 && huMoments[6] <= hu_max6_3)
    {
        cout << "jest to 3" << endl;
        cv::Point center = box.center;                                  // wyznaczenie srodka karty poprzez minAreaRect
        cout << "o wspolrzednych " << center << endl;

        kolor = jaki_kolor_karta(blue_threshold, red_threshold, green_threshold, yellow_threshold, center);
        //cout << kolor << endl;
        switch (kolor)
        {
        case 1:
            suma_b = suma_b + 3;
            break;
        case(2):
            suma_r = suma_r + 3;
            break;
        case(3):
            suma_g = suma_g + 3;
            break;
        case(4):
            suma_y = suma_y + 3;
            break;
        }

        suma = suma + 3;
    }
    if (huMoments[2] >= hu_min_arrow && huMoments[2] <= hu_max_arrow && huMoments[1] >= hu_min1_arrow && huMoments[1] <= hu_max1_arrow && (huMoments[6] <= hu_bot6_arrow || huMoments[6] >= hu_top6_arrow))
    {
        cout << "jest to strzałka" << endl;

        cv::Point center = box.center;                                  // wyznaczenie srodka karty poprzez minAreaRect
        cout << "o wspolrzednych " << center << endl;

        kolor = jaki_kolor_karta(blue_threshold, red_threshold, green_threshold, yellow_threshold, center);
        //cout << kolor << endl;
    }

}


void Segmentacja_i_detekcja_karty(std::vector<std::vector<cv::Point>>& contours, cv::Mat threshed_shapes,cv::Mat& symbol,cv::Mat& shapes, double& suma, double& suma_b, double& suma_r, double& suma_g, double& suma_y, cv::Mat blue_threshold, cv::Mat red_threshold, cv::Mat green_threshold, cv::Mat yellow_threshold)
{

    for (int i = 0; i < contours.size(); i++)
    {
        if (contours[i].size() > 30)                                 //aby pominac kontury ktore nie sa kartami
        {
            std::vector<cv::Point> cnt = contours[i];               // wybór karty do badania
            cv::RotatedRect box = cv::minAreaRect(cnt);            // najmniejszy obrócony kwadrat obejmujący kontury
            cv::Point2f points[4];                                   // wyznaczenie wierzchołków karty
            box.points(points);

            cv::Point2f corner1, corner2, corner3;
            corner1 = points[0];
            corner2 = points[1];
            corner3 = points[2];

            double sum1, sum2;
            sum1 = cv::norm(corner2 - corner1);
            sum2 = cv::norm(corner2 - corner3);

            double y_dist;
            double x_dist;

            if (sum1 > sum2)                              //określenie położenia punktów
            {
                y_dist = sum1;
                x_dist = sum2;
            }
            else
            {
                y_dist = sum2;
                x_dist = sum1;
            }


            cv::Point2f tmp1, tmp2, tmp3;

            tmp1 = corner2;

            if (sum1 > sum2)
            {
                tmp2 = corner1;
                tmp3 = corner3;
            }
            else
            {
                tmp2 = corner3;
                tmp3 = corner1;
            }


            cv::Mat result = cv::Mat::zeros(500, 500, CV_8U);
            std::vector<cv::Point2f>src_points_affine{ tmp1,tmp2,tmp3 };     //przy operacji minarearect przypisywane sa punkty zgodnie ze wskazówkami zegara
                                                                                     // wiec dla src_points_affine przypisujemy corner 2 jako pierwszy


            std::vector<cv::Point2f>dst_points_affine{ cv::Point2f(0,y_dist),cv::Point2f(0,0), cv::Point2f(x_dist,y_dist) };
            cv::Mat T = cv::getAffineTransform(src_points_affine, dst_points_affine);
            cv::warpAffine(threshed_shapes, result, T, threshed_shapes.size());
            result = result(cv::Rect(0, 0, x_dist, y_dist));

            cv::Mat symbol = cv::Mat::zeros(500, 500, CV_8U);







            int width = result.size().width;                               
            int height = result.size().height;

            symbol = result(cv::Rect(width / 4, height / 4, width / 2, height / 2));               //wysegmentowanie centralnego symbolu
            //cv::imshow("test",symbol);
            

            detekcja_symbolu(symbol,shapes,box,suma,suma_b,suma_r,suma_g,suma_y, blue_threshold, red_threshold, green_threshold, yellow_threshold);
            
            /*cout << "Nacisnij dowolny przycisk aby kontynuowac" << endl;
            cv::waitKey(0);*/
           
        }
    }
}



int main()
{
    
    cv::Mat img, threshed_shapes, shapes, blue_threshold, red_threshold, green_threshold,yellow_threshold,symbol;
    double suma=0;
    double suma_b = 0;
    double suma_r = 0;
    double suma_g = 0;
    double suma_y = 0;

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    

    WgranieObrazu(img);

    pre_processing(img, threshed_shapes, shapes);

    kontury(threshed_shapes, shapes, contours, hierarchy);

    detekcja_kolorow(img, blue_threshold, red_threshold, green_threshold, yellow_threshold);

    Segmentacja_i_detekcja_karty(contours,threshed_shapes,symbol,shapes,suma, suma_b, suma_r, suma_g, suma_y, blue_threshold, red_threshold, green_threshold, yellow_threshold);

    cout << "suma wszystkich kart wynosi" << endl;
    cout<< suma << endl;


    cout << "suma niebieskich kart wynosi" << "  " << suma_b << endl;
    cout << "suma czerwonych kart wynosi" << "  " << suma_r << endl;
    cout << "suma zielonych kart wynosi" << "  " << suma_g << endl;
    cout << "suma zoltych kart wynosi" << "  " << suma_y<< endl;

    return(0);
}

