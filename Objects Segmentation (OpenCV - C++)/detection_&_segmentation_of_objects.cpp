#include <iostream>
#include <string>
#include <sstream>
#include <random>
using namespace std;


#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;

const char* keys =
{
  "{help h usage ? | | print this message}"
  "{@image || Image to process}"
  "{@lightPattern || Image light pattern to apply to image input}"
  "{lightMethod | 1 | Method to remove background light, 0  difference, 1 div }"
  "{segMethod | 1 | Method to segment: 1 connected Components, 2 connected components with stats, 3 find Contours }"
};


static Scalar randomColor( RNG& rng )
  {
    int icolor = (unsigned) rng;
    return Scalar( icolor&255, (icolor>>8)&255, (icolor>>16)&255 );
  }

/*----------------------------------------------------------*/
// calculate approx.light pattern or background approximation
/*----------------------------------------------------------*/

Mat calculateLightPattern(Mat img)
{
  Mat pattern;
  blur(img, pattern, Size(img.cols/3,img.cols/3));
  return pattern;
}

/*----------------------------------------------------------*/
//            Remove light WITH light pattern
/*----------------------------------------------------------*/
Mat removeLight(Mat img, Mat background_pattern, int method)
{
  Mat res;

  // if method is normalization
  if(method==1) // method == 1 -> division R = 255*(1-(I/L))
  {

    Mat img32, background_pattern32;
    img.convertTo(img32, CV_32F);
    background_pattern.convertTo(background_pattern32, CV_32F);
    // Divide the image by the background_pattern
    res= 1-(img32/background_pattern32);
    // Scale it to convert to 8bit format
    res=res*255;
    // Convert 8 bits format
    res.convertTo(res, CV_8U);
  }

  else // method != 0 -> difference R = L-I
  {
    res= background_pattern-img;
  }
  return res;
}
/*----------------------------------------------------------*/
//                     Colore the objects
/*----------------------------------------------------------*/
Mat ConnectedComponents(Mat img)
{
  // Use connected components to divide our possibles parts of images
  Mat labels;
  int num_objects = connectedComponents(img, labels);

  // Check the number of objects detected
  if(num_objects < 2 )
  {
    cout << "No objects detected" << endl;
  }
  else
  {
    cout << "Number of objects detected: " << num_objects - 1 << endl;
  }

  // Create output image coloring the objects
  Mat output= Mat::zeros(img.rows,img.cols, CV_8UC3);
  RNG rng( 0xFFFFFFFF );

  for(int i=1; i<num_objects; i++)
  {
    Mat mask= labels==i;
    output.setTo(randomColor(rng), mask);
  }

  return output;
}

/*----------------------------------------------------------*/
//  Colore the objects and draw the area of the objects
/*----------------------------------------------------------*/

Mat ConnectedComponentsStats(Mat img)
{
  // Use connected components with stats
  Mat labels, stats, centroids;
  int num_objects = connectedComponentsWithStats(img, labels, stats, centroids);

  // Check the number of objects detected
  if(num_objects < 2 )
  {
    cout << "No objects detected" << endl;
  }
  else
  {
    cout << "Number of objects detected: " << num_objects - 1 << endl;
  }

  // Create output image to colore the objects and show area
  Mat output = Mat::zeros(img.rows,img.cols, CV_8UC3);
  RNG rng( 0xFFFFFFFF );

  for(int i=1; i<num_objects; i++)
  {
    // for each label that we detect, we will show its centroid and area
    cout << "Object "<< i << " with pos: " << centroids.at<Point2d>(i) << " with area " << stats.at<int>(i, CC_STAT_AREA) << endl;

    //color the objects
    Mat mask = labels==i;
    output.setTo(randomColor(rng), mask);

    // draw text with area
    stringstream ss;
    ss << "area: " << stats.at<int>(i, CC_STAT_AREA);
    putText(output, ss.str(), centroids.at<Point2d>(i), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255,255,255));
  }

  return output;
}

/*----------------------------------------------------------*/
//  draw contours with the "findContours" function
/*----------------------------------------------------------*/
Mat FindContoursBasic(Mat img)
{
  vector<vector<Point> > contours;

  //ATTENTION : 'img' will be modified !! -> we should do a backup if necessary

  findContours(img, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

  Mat output= Mat::zeros(img.rows,img.cols, CV_8UC3);

  if(contours.size() == 0 )
  {
    cout << "No objects detected" << endl;
  }
  else
  {
    cout << "Number of objects detected: " << contours.size() << endl;
  }

  RNG rng( 0xFFFFFFFF );
  for(int i=0; i<contours.size(); i++)
  drawContours(output, contours, i, randomColor(rng));

  return output;
}

/*----------------------------------------------------------*/
//                  Main
/*----------------------------------------------------------*/
int main( int argc, const char** argv )
{

  CommandLineParser parser(argc, argv, keys);
  parser.about("Chapter 5. PhotoTool v1.0.0");

  if (parser.has("help"))
  {
    parser.printMessage();
    return 0;
  }

  string img_file;
  string light_pattern_file;
  int method_light;
  int method_seg;

  printf("\nOutput image : ");
  getline (cin, img_file);
  // Load image to process
  Mat img= imread(img_file, 0);
  if(img.data==NULL)
  {
    cout << "Error loading image "<< img_file << endl;
    return 0;
  }

  printf("\nBackground : ");
  getline (cin, light_pattern_file);

  printf("\nMethod for removing the light\n0. Soustraction\n1. Division");
  printf("\n2. Don't remove : the image has a white background and the object images have lower values (~dark)\n\nChoice : ");
  scanf("%d", &method_light);
  printf("\nMethod for segmentation\n1. Connected Components\n2. Connected components with stats\n3. find Contours function\n\nChoice : ");
  scanf("%d", &method_seg);
  printf("\n\n");



  cout << "Dimension : rows = " << img.rows << " & columns = " << img.cols << endl;
  printf("\n\n");

  /*----------------------------------------*/
  //                Noise removal
  /*----------------------------------------*/

  Mat img_noise;
  medianBlur(img, img_noise, 3); /


   /*----------------------------------------*/
   //         Light / background removal
   /*----------------------------------------*/

   Mat pattern= imread(light_pattern_file, 0);
   if(pattern.data==NULL)
   {
     pattern = calculateLightPattern(img_noise);
   }

  namedWindow("Input");
  namedWindow("Noise removed");
  namedWindow("background");

  moveWindow("Input", 10, 10);
  moveWindow("Noise removed", 20  + img.cols, 10);
  moveWindow("background", 30 + 2 * img_noise.cols , 10);

  imshow("Input", img);
  imshow("Noise removed", img_noise);
  imshow("background", pattern);

  Mat img_no_light;

  if (method_light==0) {
    namedWindow("background removed - sosutraction");
    moveWindow("background removed - sosutraction", 10 , 70 + img.rows);

    img_no_light = removeLight(img_noise, pattern, 0);
    imshow("background removed - sosutraction", img_no_light);}

  else {
    namedWindow("background removed - division");
    moveWindow("background removed - division", 10 , 70 + img.rows);

    img_no_light = removeLight(img_noise, pattern, 1);
    imshow("background removed - division", img_no_light);}


  /*----------------------------------------*/
  //             Thresholding
  /*----------------------------------------*/

  // Binarize image for segment
  Mat img_thr;

  if(method_light!=2)
  {
    /* when we have removed the light/background :
      all non-interest regions are black or very low values ~0 */
    threshold(img_no_light, img_thr, 30, 255, THRESH_BINARY);
  }
  else // method_light == 2, no light/background remove was done :
  {   // when we have a white background and the object images have lower values
    threshold(img_no_light, img_thr, 140, 255, THRESH_BINARY_INV);
  }


 namedWindow("Thresholding after division");
 moveWindow("Thresholding after division", 20  + img.cols , 70 + img.rows);
 imshow("Thresholding after division",  img_thr);

 /*----------------------------------------*/
 //             Segmentation
 /*----------------------------------------*/


if (method_seg==1) {
  namedWindow("Objets marqués");
  moveWindow("Objets marqués", 30  + 2 * img.cols , 70 + img.rows);

  Mat obj_couleur;
  obj_couleur = ConnectedComponents(img_thr);
  imshow("Objets marqués",  obj_couleur);}

if (method_seg==2) {
  namedWindow("Objets marqués & aire des objets");
  moveWindow("Objets marqués & aire des objets", 30  + 2 * img.cols , 70 + img.rows);

  Mat obj_couleur_aire;
  obj_couleur_aire = ConnectedComponentsStats(img_thr);
  imshow("Objets marqués & aire des objets", obj_couleur_aire);}

if (method_seg==3) {
  namedWindow("Résultat de 'findContours'");
  moveWindow("Résultat de 'findContours'", 30  + 2 * img.cols , 70 + img.rows);

  Mat obj_contours;
  obj_contours=FindContoursBasic(img_thr);
  imshow("Résultat de 'findContours'", obj_contours);}

  waitKey(0);
  return 0;
}
