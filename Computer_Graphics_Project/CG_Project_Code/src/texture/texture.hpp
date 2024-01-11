// /**
//  * @file texture.hpp
//  * @brief Texture Synthesis with Blending and Error Minimization
//  */
// #include <bits/stdio.h>
// #include <opencv2/opencv.hpp>
// #include <random>
// using namespace cv;
// using namespace std;

// // int generate_solid_texture(const char* exemplar_file) {
// int generate_solid_texture(std::string& exemplar_file){  
//     Mat3b input_texture;

//     //Read the exemplar using OpenCV's imread function using Coloured image mode
//     input_texture = cv::imread(exemplar_file, IMREAD_COLOR);

//     // Input texture is [128*128]
//     int exemplar_width=input_texture.size().width;
//     int exemplar_height=input_texture.size().height;

//     srand(time(0));

// // --------------------------------------------------------------------------------------------
//     // Extract the number from the input filename
//     size_t start = exemplar_file.find_last_of("_") + 1;
//     size_t end = exemplar_file.find_last_of(".");
//     std::string number_str = exemplar_file.substr(start, end - start);

//     // Convert the extracted number string to an integer
//     int exemplar_number;
//     std::istringstream(number_str) >> exemplar_number;
// // ----------------------------------------------------------------------------------------------

//     //Define the output texture having 1024 rows and columns and 3 channels(rgb)
//     Mat3b output_solid_texture(1024, 1024, Vec3b(0, 0, 0));

//     // Fill in for 1024*1024 output solid texture
//     //Moving by 64,i.e., [8*8] which is recommended in the research paper to be implemented{Overview Para-3}

//     int x,y;

//     for(int i=0;i<1024;i+=64) {
//         for(int j=0;j<1024;j+=64) {

//             //Select random point from exemplar, notice that -64 is being done to prevent out of bounds situation
//             x =rand()%(exemplar_width-64);
//             y =rand()%(exemplar_height-64);

//             //Run limited iterations
//             int it=0;
//             while(it<15){
            
//                 //Defines the patch (Tune values as appropriate)
//                 int starting_pointX=max(0,min(exemplar_width-64,x-90+(rand()%128)));
//                 int starting_pointY=max(0,min(exemplar_height-64,y-90+(rand()%128)));

//                 for(int k=0;k<64;++k){
//                     for(int l=0;l<64;++l){
//                         output_solid_texture(i+k,j+l)=input_texture(starting_pointY+k,starting_pointX+l);
//                     }
//                 }
                
//                 //Here need to put any optimization such as MSE so that we know if the errors are being minimized
//                 //as iterations happen

//                 it++;
//             }
//         }
//     }

// // ---------------------------------------------------------------------------------------------------------------------
//     // imwrite("./texture_outputs/output_1.jpg",output_solid_texture);
//     //  Create the output filename with the extracted number
//     std::ostringstream output_filename;
//     output_filename << "./texture_outputs/output_" << exemplar_number << ".jpg";

//     cv::imwrite(output_filename.str(), output_solid_texture);

// // ---------------------------------------------------------------------------------------------------------------------
//     return 0;
// }

// -------------------------------------------------------------------------------------------------After Mid Evaluation -------------------------------------------------------------------------------------
/**
 * @file texture.hpp
 * @brief Texture Synthesis with Blending and Error Minimization
 */
#include <bits/stdio.h>
#include <opencv2/opencv.hpp>

#include <random>
using namespace cv;
using namespace std;

// Reference for Histogram Matching (OpenCV documentation) --> "https://docs.opencv.org/4.x/d8/dc8/tutorial_histogram_comparison.html"

//Get the histogram matching score between the input texture and the patch generated
double histogram_matching(Mat3b original,Mat3b patch){
    if( original.empty() || patch.empty())
    {
        cout << "Cannot find the images\n" << endl;
        return -1;
    }
    //Convert BGR to HSV format  (HSV separates color information (hue and saturation) from brightness (value))
    Mat hsv_original, hsv_patch;
    cvtColor( original, hsv_original, COLOR_BGR2HSV );
    cvtColor( patch, hsv_patch, COLOR_BGR2HSV );

    //Initialize the histograms with parameters
    int h_bins = 50,s_bins = 60;
    int histSize[] = {h_bins,s_bins};

    // hue varies from 0 to 179, saturation from 0 to 255
    float h_ranges[]={0,180};
    float s_ranges[]={0,256};

    const float* ranges[]={h_ranges,s_ranges};

    // Use the 0-th and 1-st channels
    int channels[]={0,1};

    Mat hist_original,hist_patch;
    calcHist( &hsv_original, 1, channels, Mat(), hist_original, 2, histSize, ranges, true, false );
    normalize( hist_original, hist_original, 0, 1, NORM_MINMAX, -1, Mat() );

    calcHist( &hsv_patch, 1, channels, Mat(), hist_patch, 2, histSize, ranges, true, false );
    normalize( hist_patch, hist_patch, 0, 1, NORM_MINMAX, -1, Mat() );

    //Calculate the score with method Chi-Square( corresponding to parameter 1 below)
    double base_base = compareHist( hist_original, hist_original,1);
    double base_test1 = compareHist( hist_original, hist_patch,1);

    //Lower the score, better the match
    cout<<"To itself"<<base_base<<endl;
    cout<<"compare"<<base_test1<<endl;
    // return 0;
    return base_test1;
}

std::string get_plane_name(int plane) {
    switch (plane) {
        case 1: return "yz";
        case 2: return "xz";
        case 3: return "xy";
        default: return "unknown";
    }
}

// int generate_solid_texture(const char* exemplar_file) {
int generate_solid_texture(std::string& exemplar_file){  
    Mat3b input_texture,prev_patch;

    //Read the exemplar using OpenCV's imread function using Coloured image mode
    input_texture = cv::imread(exemplar_file, IMREAD_COLOR);

    // Input texture is [128*128]
    int exemplar_width=input_texture.size().width;
    int exemplar_height=input_texture.size().height;

    srand(time(0));

// --------------------------------------------------------------------------------------------
    // Extract the number from the input filename
    size_t start = exemplar_file.find_last_of("_") + 1;
    size_t end = exemplar_file.find_last_of(".");
    std::string number_str = exemplar_file.substr(start, end - start);

    // Convert the extracted number string to an integer
    int exemplar_number;
    std::istringstream(number_str) >> exemplar_number;
// ----------------------------------------------------------------------------------------------

    // //Define the output texture having 1024 rows and columns and 3 channels(rgb)
    Mat3b output_solid_texture(1024, 1024, Vec3b(0, 0, 0));
    // Fill in for 1024*1024 output solid texture
    //Moving by 64,i.e., [8*8] which is recommended in the research paper to be implemented{Overview Para-3}

    int x,y;
    Mat3b patch_prev(64,64, Vec3b(0, 0, 0));
    Mat3b cur_patch(64,64, Vec3b(0, 0, 0));

    int threshold = 14;

    // Add loops for yz, xz, and xy planes
    for (int plane = 1; plane <= 3; ++plane) {
        for(int i=0;i<1024;i+=64) {
            for(int j=0;j<1024;j+=64) {
                //Select random point from exemplar, notice that -64 is being done to prevent out of bounds situation
                x =rand()%(exemplar_width-64);
                y =rand()%(exemplar_height-64);


                int starting_pointX,starting_pointY;

                int error=0;
                int mse_it=0;

                do{
                    //Defines the patch (Tune values as appropriate)
                    starting_pointX=max(0,min(exemplar_width-64,x-90+(rand()%128)));
                    starting_pointY=max(0,min(exemplar_height-64,y-90+(rand()%128)));

                    //Patch in current iteration
                    for(int k=0;k<64;++k){
                        for(int l=0;l<64;++l){
                            cur_patch(k,l)=input_texture(starting_pointY+k,starting_pointX+l);
                        }
                    }

                    double score = histogram_matching(input_texture,cur_patch);
                    
                    if(score < 0.55) {
                        threshold *= 1.5;
                        mse_it++;
                        continue;
                    }

    //-------------------------ERROR CALCULATION-----------------------------
                
                    Mat3b mse_matrix;

                    absdiff(cur_patch,patch_prev,mse_matrix);
                    
                    //Square component
                    Mat3b mse_matrix_square=mse_matrix.mul(mse_matrix);

                    //Sum of the matrix(essentially the sum of squared values )
                    Scalar mse_sum = cv::sum(mse_matrix_square);

                    //Height * width * num_of_channels
                    double mean_component=64*64*3;

                    //Error to compare if new patches are better and apply threshold on it

                    // RMSE the 3 channels 
                    double error = sqrt((mse_sum[0] + mse_sum[1] + mse_sum[2]) / mean_component);
                    cout<<error<<endl;

    //------------------------------------------------------------------------
                    //Maximum iterations allowed
                    mse_it++;
                }
                while(error>threshold || mse_it<100);


                //Modify the output solid texture with the best patch
                for(int k=0;k<64;++k){
                    for(int l=0;l<64;++l){
                        output_solid_texture(i+k,j+l)=input_texture(starting_pointY+k,starting_pointX+l);
                    }
                }

                //Update the previous patch for the next iteration
                prev_patch=cur_patch;

            }
        }
      // Write images to their respective folders
        std::ostringstream output_filename;
        output_filename << "./texture_outputs_" << get_plane_name(plane) << "/output_" << exemplar_number << ".jpg";
        cv::imwrite(output_filename.str(), output_solid_texture);
   }

// ---------------------------------------------------------------------------------------------------------------------
    // imwrite("./texture_outputs/output_1.jpg",output_solid_texture);
    //  Create the output filename with the extracted number
    // std::ostringstream output_filename;
    // output_filename << "./texture_outputs/output_" << exemplar_number << ".jpg";

    // cv::imwrite(output_filename.str(), output_solid_texture);

// ---------------------------------------------------------------------------------------------------------------------
    return 0;
}



