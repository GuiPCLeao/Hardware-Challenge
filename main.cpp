/**
 * Created by: Guilherme P.C. Leão
 *
 * REVIEWING:
 *
 * DFT{x} = X;
 * X[k] = \sum_{n = 0}^{N} {x[n] W_{N}^{n*k}}
 * 
 * - x is the input vector;
 * - X is the DFT of x;
 * - N is the size of x;
 * - W_{N} = \exp{-j*2*pi/N}.
 * - n,k = 0, 1, 2, ... , N-1
 *  
 */
//#include <iostream>
#include <string>
#include <iomanip> 
#include <vector>
#include <fstream>
#include <sstream>
#include <fftw3.h> // download at: http://www.fftw.org/download.html
#include <math.h>

#define REAL 0 // index of real components in fftw_complex typedef
#define IMAG 1 // index of imaginary component in fftw_complex typedef
#define MAG_FLOAT_PRECISION 12
#define FREQ_FLOAT_PRECISION 3

const char* inputFileName = "1602245833-2715-NAO7856.txt";
const char* outputFileName = "output.txt";

/**
 * Get x, y and z values in a row from a filestream.
 * @param fileStream the filestream of input file.
 * @return vector of float containing the x, y and z values of a row.
 */
std::vector<float> getRowValues(std::ifstream&);//, std::vector<float>&, std::vector<float>&, std::vector<float>&);

/**
 * Compute the DFT magnitudes of an input vector.
 * @param vec the input vector.
 * @param vecDFT the magnitudes of the input vector's DFT.
 */
void getDFT_magnitudes(const std::vector<float>&, std::vector<float>&);

int main() {
  /* FIRST STEP - CREATE X, Y AND Z VECTORS */
  std::vector<float> xCoords, yCoords, zCoords;

  std::ifstream sensorDataFile;
  std::ofstream dftDataFile;
  sensorDataFile.open(inputFileName);
  
  /* Parsing the input file */
  if (sensorDataFile.is_open()) {
    std::string txtHeader;
    std::getline(sensorDataFile, txtHeader); // discarting the txt header ("x,y,z")

    std::vector<float> coords;
    while (!sensorDataFile.eof()) {
      coords = getRowValues(sensorDataFile); // parsing each row at a time
      if (coords.size() > 0) {
        xCoords.push_back(coords[0]);
        yCoords.push_back(coords[1]);
        zCoords.push_back(coords[2]);
      }
    }
  }
  sensorDataFile.close();

  /* SECOND STEP - COMPUTING X, Y AND Z DFTs */
  /* Computing input data relevant parameters */
  int N = xCoords.size(); // Sample size
  const float collectionTime = 2715.0f; // Collection duration in milliseconds
  float T = collectionTime/(N-1); // Sample period in milliseconds
  float fs = (1.0f/T) * 1000; // Sample frequency in Hz

  /* Computing DFT magnitudes */
  std::vector<float> xDFT, yDFT, zDFT;
  getDFT_magnitudes(xCoords, xDFT);
  getDFT_magnitudes(yCoords, yDFT);
  getDFT_magnitudes(zCoords, zDFT);

  /* THIRD STEP - EXPORTING DFTs DATA */
  dftDataFile.open(outputFileName);
  for(int i = 0; i < N; i++) {
    dftDataFile << std::fixed << std::setprecision(MAG_FLOAT_PRECISION) // magnitudes with 12 digits precision
                << xDFT[i] << "," 
                << yDFT[i] << "," 
                << zDFT[i] << "," 
                << std::fixed << std::setprecision(FREQ_FLOAT_PRECISION) // frequencies with 3 digits precision
                << i * fs/N << std::endl;
  }
  dftDataFile.close();

  return 0;
}

void getDFT_magnitudes(const std::vector<float> &vec, std::vector<float> &vecDFT) {
  int N = vec.size(); // sample size
  fftw_complex inputComplex[N]; // complex array of vec
  fftw_complex outputComplex[N]; // complex array of vecDFT
  fftw_plan plan;

  /* Creating input complex array */
  for(int n = 0; n < N; n++) {
    inputComplex[n][REAL] = vec[n];
    inputComplex[n][IMAG] = 0;
  }

  plan = fftw_plan_dft_1d(N, inputComplex, outputComplex, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(plan);
  fftw_destroy_plan(plan);
  fftw_cleanup();

  /* Computing the magnitute of each frequency component */
  for(int k = 0; k < N; k++) {
    vecDFT.push_back(sqrt(outputComplex[k][REAL]*outputComplex[k][REAL] + 
                          outputComplex[k][IMAG]*outputComplex[k][IMAG]));
  }
}

std::vector<float> getRowValues(std::ifstream &fileStream) {
  std::vector<float> coords; // returned coords

  std::string row, cell;
  std::stringstream rowStream;

  std::getline(fileStream, row); // current row (being parsed)
  rowStream << row;

  while (std::getline(rowStream, cell, ',')) { // parsing each row's cell (x, y and z axis)
    coords.push_back(std::stof(cell));
  }
  return coords;
}
