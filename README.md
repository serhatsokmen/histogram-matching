# histogram-matching
This project manually calculates the Histogram and Cumulative Distribution Function (CDF) for each RGB channel and uses a minimum absolute difference method to generate the mapping table.


## Project Overview
This project is a manual implementation of the Histogram Specification algorithm in C++.
The primary goal is to match the color distribution (histoggram) of an input image to that of a designated reference image.
It was developed with a strict constraint: to implement all core mathematical operations (Histogram, CDF and Mapping) without using built-in-high-level OpenCV functions.

## Technical Details and Methodology
The akgorithm operates on the color channels independently to ensure accurate color preservation. 
The input RGB image is first split into its Blue, Green and Red (BGR)channels using ```cv::split```.
The transformation process is executed channel-by-channel (B, G, then R). 

### Core Function (Manual Implementation)
The entire logic relies on the following four manually coded functions:

#### 1. ```calculate_hist_and_cdf```: 
This function manually counts the pixels for each intensity level (0-255) to create the Histogram.
It then calculates the Cumulative Distribution Function (CDF), which is the mathematical basis for the matching process.
#### 2. ```create_mapping_table```:
It compares the input CDF against the Reference CDF to find the best match for every pixel value.
The function generates a 256-element mapping table based on the minimum absolute difference between the two CDFs. 
#### 3. ```apply_mapping```:
This function takes the original image channel and the generated mapping table.
It applies the transformation by replacing each pixel's old value with the new value stored in the mapping table.
#### 4. ```draw_histogram```:
Used for visual analysis, this utility function draws the calculated histogram data onto an OpenCV window.

## Results and Visual Comparison
The output image succesfully matches the luminosity and color contrast characteristics of the reference image.
The final image (output_matched.jpg) is generated when the program is run. 
