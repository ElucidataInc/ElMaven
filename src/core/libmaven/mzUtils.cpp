#include <thread>

#ifdef UNIX
#include <unistd.h>
#endif
#ifdef WIN32
#include <windows.h>
#endif

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/algorithm/string.hpp>

#include "doctest.h"
#include "mzUtils.h"
#include "SavGolSmoother.h"
#include "csvparser.h"
#include "masscutofftype.h"
#include "RealFirFilter.h"

/**
 * random collection of useful functions 
 */
namespace mzUtils {

    int randInt(int low, int high)
    {
        default_random_engine generator;
        uniform_int_distribution<int> distribution(low, high);
        int res = distribution(generator);
        return res;
    }

    long randLong(long low, long high)
    {
        default_random_engine generator;
        uniform_int_distribution<int> distribution(low, high);
        long res = distribution(generator);
        return res;
    }

    float randFloat(float low, float high)
    {
        default_random_engine generator;
        uniform_real_distribution<float> distribution(low, high);
        float res = distribution(generator);
        return res;
    }

    double randDouble(double low, double high)
    {
        default_random_engine generator;
        uniform_real_distribution<double> distribution(low, high);
        double res = distribution(generator);
        return res;
    }

    std::string makeLowerCase(string &str)
    {
        transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }

    vector<string> split(const string& str, const string& sep)
    {
        vector<string> result;
        int start = 0;
        int end = 0;
        while(end != string::npos){
            end = str.find(sep, start);
            result.push_back(str.substr(start, end - start));
            start = end + sep.length();
        }
        return result;
    }

    vector<string> splitCSVFields(const string& s,const string& c){

        vector<string> v;
        const char *whole_row = s.c_str();
        const char *del = c.c_str();

        CsvParser *csvparser = CsvParser_new_from_string(whole_row, del, 0);

        CsvRow *row;

        row = CsvParser_getRow(csvparser);
        const char **rowFields = CsvParser_getFields(row);
        
        for (int i = 0 ; i < CsvParser_getNumFields(row) ; i++) {
            v.push_back(rowFields[i]);
        }
        return v;
    }

    string join(const vector<string>& words, const string& sep)
    {
        if (words.empty())
            return "";
        if (words.size() == 1)
            return words.at(0);

        return accumulate(next(begin(words)),
                          end(words),
                          words.at(0),
                          [sep](string a, string b) { return a + sep + b; });
    }

    void removeSpecialCharFromStartEnd(vector<string>& fields)
    {
        for(size_t i = 0; i < fields.size(); i++) {
            int n = fields[i].length();
            if (n > 2 ){
                if((fields[i][0] == '"' && fields[i][n - 1] == '"') ||
                    (fields[i][0] == '\'' && fields[i][n - 1] == '\''))
                        fields[i] = fields[i].substr(1, n - 2);
            }
        }
    }

    bool contains(const string& first,
                  const string& second,
                  const bool ignoreCase)
    {
        string s = first;
        string p = second;
        if (ignoreCase == true) {
            transform(s.begin(), s.end(), s.begin(), ::tolower);
            transform(p.begin(), p.end(), p.begin(), ::tolower);
        }

        if (boost::algorithm::contains(s, p))
            return true;
        else
            return false;
    }

    void smoothAverage(float *input, float* result, int smoothWindowLen,
                       int inputLen)
    {
        if (smoothWindowLen == 0 ) return;
        float* x = new float[smoothWindowLen];

        for(int i = 0; i < smoothWindowLen; i++)
            x[i] = 1.0/smoothWindowLen;
        conv(smoothWindowLen, -smoothWindowLen/2, x, inputLen,
                 0, input, inputLen, 0, result);
        delete[] x;
    }

    void conv (int xLen, int indexFirstX, float *x, int inputLen,
              int indexFirstInput, float *input, int resultLen,
              int indexFirstResult, float *result)
   {
        int ilx = indexFirstX + xLen - 1;
        int ily = indexFirstInput + inputLen - 1;
        int ilz = indexFirstResult + resultLen - 1;

        int i, j, jlow, jhigh;
        float sum;

        x -= indexFirstX;
        input -= indexFirstInput;
        result -= indexFirstResult;

        for (i = indexFirstResult; i <= ilz; ++i)
        {
            jlow = i - ily;
            if (jlow < indexFirstX)
                    jlow = indexFirstX;
            jhigh = i - indexFirstInput;
            if (jhigh > ilx)
                jhigh = ilx;

            for (j = jlow, sum = 0.0; j <= jhigh; ++j)
                sum += x[j] * input[i-j];
            result[i] = sum;
        }
    }

    void gaussian1d_smoothing (int numSample, int smoothWindowLen, float *data)
    {
        int is;             /* loop counter */
        float sum = 0.0;
        float fcut;
        float r;
        float fcutr = 1.0/smoothWindowLen;
        int n = 0;
        int mean = 0;
        float fcutl = 0;
        float s[1000];      /* smoothing filter array */
        float *temp;            /* temporary array */

        /* save input fcut */
        fcut = fcutr;

        /* don't smooth if nsr equal to zero */
        if (smoothWindowLen == 0 || numSample <= 1)
            return;

        /* if halfwidth more than 100 samples, truncate */
        if (smoothWindowLen > 100)
            fcut=1.0/100;

        /* allocate space */
        temp = new float[numSample];

        /* initialize smoothing function if not the same as the last one used */
        if (fcut != fcutl) {
            fcutl = fcut;

            /* set span of 3, at width of 1.5*exp(-PI*1.5**2)=1/1174 */
            n = (int) (3.0 / fcut + 0.5);
            n = 2 * n / 2 + 1;      /* make it odd for symmetry */

            /* mean is the index of the zero in the smoothing wavelet */
            mean = n / 2;

            /* s(n) is the smoothing gaussian */
            for (is = 1; is <= n; is++) {
                r = is- mean - 1;
                r = -r * r * fcut * fcut * 3.141;
                s[is-1] = exp(r);
            }

            /* normalize to unit area, will preserve DC frequency at full
               amplitude. Frequency at fcut will be half amplitude */
            for (is = 0; is < n; is++)
                sum += s[is];
            for (is = 0; is < n; is++)
                s[is] /= sum;
        }

        /* convolve by gaussian into buffer */
        if (1.01/fcutr > (float)numSample) {
            /* replace drastic smoothing by averaging */
            sum = 0.0;
            for (is = 0; is < numSample; is++)
                sum += data[is];
            sum /= numSample;

            for (is = 0; is < numSample; is++)
                data[is] = sum;

        } else {
            /* convolve with gaussian */
            conv (n, -mean, s, numSample, -mean, data, numSample, -mean, temp);
            /* copy filtered data back to output array */
            for (is = 0; is < numSample; is++) data[is] = temp[is];
        }
        /* free allocated space */
        delete[] temp;
    }

    float median(vector <float> y)
    {
        if (y.empty())
            return(0.0);
        if (y.size() == 1 )
            return(y[0]);
        if (y.size() == 2 ){
            return(y[0] + y[1])/2;
        }

        unsigned int n = y.size();
        std::sort(y.begin(), y.end());

        unsigned int i = n / 2;
        float median = 0.0;

        if (n == i * 2)
            median = (y[i-1] + y[i]) / 2.;
        else
            median = y[i];

        return (median);
    }

    int string2integer(const std::string& s)
    {
        std::istringstream i(s);
        int x = 0;
        i >> x;
        return x;
    }

    float string2float(const std::string& s)
    {
        std::istringstream i(s);
        float x = 0;
        i >> x;
        return x;
    }

    string integer2string(int x)
    {
        std::stringstream i;
        string s;
        i << x;
        i >> s;
        return s;
    }

    string float2string(float f, int p)
    {
        std::stringstream ss;
        ss << setprecision(p) << f; string str; ss >> str;
        return(str);
    }

    float massCutoffDist(const float mz1, const float mz2,
                         MassCutoff *massCutoff)
    {
        if(massCutoff->getMassCutoffType() == "ppm"){
            return (abs((mz2-mz1)/(mz1/1e6)));
        }
        else if(massCutoff->getMassCutoffType() == "mDa"){
            return abs((mz2-mz1)*1e3) ;
        }
        else{
            assert(false);
            return 0;
        }
    }

    double massCutoffDist(const double mz1, const double mz2,
                          MassCutoff *massCutoff)
    {
        if(massCutoff->getMassCutoffType() == "ppm"){
            return ppmDist(mz1, mz2);
        }
        else if(massCutoff->getMassCutoffType() == "mDa"){
            return abs((mz2-mz1)*1e3) ;
        }
        else{
            assert(false);
            return 0;
        }
    }

    float ppmDist(const float mz1, const float mz2)
    {
        return (abs((mz2 - mz1) / (mz1 / 1e6)));
    }

    double ppmDist(const double mz1, const double mz2)
    {
        return (abs((mz2 - mz1) / (mz1 / 1e6)));
    }

    float ppmround(const float mz1, const float resolution)
    {
        return( round(mz1 * resolution) / resolution);
    }

    bool withinXMassCutoff( float mz1, float mz2, MassCutoff *massCutoff )
    {
        float masscutOffMz = massCutoff->massCutoffValue(mz1);
        if ( mz2 > (mz1 - masscutOffMz) &&
            mz2 < (mz1 + masscutOffMz) ){
            return(true);
        } else
            return(false);
    }


    vector<float> quantileDistribution( vector<float> input)
    {
        int inputSize = input.size();
        std::sort(input.begin(), input.end());
        vector<float> quantiles(101, 0);

        for (int i = 0; i < 101; i++ ) {
            int pos = i/100.0 * inputSize ;
            if (pos < inputSize)
                quantiles[i] = input[pos];
        }
        return(quantiles);
    }


    float ttest(StatisticsVector<float>& groupA,
                                 StatisticsVector<float>& groupB )
    {
        int n1 = groupA.size();
        int n2 = groupB.size();
        if ( n1 == 0 && n2 == 0)
            return 0;
        if ( n1 == 0 || n2 == 0)
            return 1000;
        float meanA = groupA.mean();
        float meanB = groupB.mean();

        float stdA  = groupA.stddev(meanA);
        float stdB  = groupB.stddev(meanB);
        if (stdA == 0 ) stdA=1.0;
        if (stdB == 0 ) stdB=1.0;

        float t_test = (meanA - meanB) / sqrt(((stdA * stdA) / n1) +
                                              ((stdB * stdB) / n2));
        return t_test;
    }

    bool fileExists(string strFilename)
    {
        struct stat stFileInfo;
        bool flag;
        int intStat;
        intStat = stat(strFilename.c_str(), &stFileInfo);
        if(intStat == 0) {
            flag = true;
        } else {
            flag = false;
        }
        return(flag);
    }


    int createDir(string path) {
        if (isDir(path)) return 0;
        cout << "Creating path: " << path << endl;
        mode_t old_mask = umask(0);
#ifdef MINGW
        int retval = mkdir(path.c_str());
#else
        int retval = mkdir(path.c_str(), 0771);
#endif
        umask(old_mask);
        return retval;
    }

    int isDir(string path) {
        struct stat sbuf;
        int retval = stat(path.c_str(), &sbuf);
        return (!retval && (sbuf.st_mode & S_IFDIR));
    }

    bool almostEqual(double a, double b, double epsilon)
    {
        // the machine epsilon has to be scaled to the magnitude of the values
        // used and multiplied by the desired precision in ULPs (units in the
        // last place).
        return std::abs(a - b) <= epsilon
                * std::abs(a + b)
                * 2
                // unless the result is subnormal
                || std::abs(a - b) < std::numeric_limits<double>::min();
    }

    bool almostEqual(float a, float b, float epsilon)
    {
        // the machine epsilon has to be scaled to the magnitude of the values
        // used and multiplied by the desired precision in ULPs (units in the
        // last place).
        return std::abs(a - b) <= epsilon
                * std::abs(a + b)
                * 1
                // unless the result is subnormal
                || std::abs(a - b) < std::numeric_limits<float>::min();
    }

    float correlation(const vector<float>&x, const vector<float>&y)
    {
        int n = x.size();
        double sumx = 0;
        double sumy = 0;
        double sumxy =0;
        double x2 = 0;
        double y2 = 0;

        for (int i = 0; i < n; i++) {
            sumx += x[i];
            sumy += y[i];
            sumxy += x[i] * y[i];
            x2 += x[i] * x[i];
            y2 += y[i] * y[i];
        }
        if (n == 0) return 0;
        double var1 = x2 - (sumx * sumx) / n;
        double var2 = y2 - (sumy * sumy) / n;
        if ( var1 == 0 || var2 == 0 ) return 0;
        float corr = (sumxy -( sumx * sumy) / n) /
                       sqrt((x2 - (sumx * sumx) / n) *
                          (y2 - (sumy * sumy) / n));
        return corr;
    }

    pair<float, float> gaussFit(const vector<float>& ycoord)
    {
        float s = 20;
        float min_s = 0;
        float minR = 1e99;

        //find best fit
        if (ycoord.size()<3){
            pair<float, float> res = make_pair(numeric_limits<float>::max(),
                                               numeric_limits<float>::max());
            return res;
        }
        vector<float>yobs = ycoord;
        int ysize = yobs.size();
        int midpoint = int(ysize/2);
        //find maximum point ( assuming it somewhere around midpoint of the yobs);
        float ymax = max(max(yobs[midpoint], yobs[midpoint-1]),
                         yobs[midpoint+1]);
        float ymin = min( yobs[0], yobs[ysize-1]);

        //initialize x vector, values centered around 0,
        //forxample  -2, -1, 0, 1, 2
        int xinit = int(ysize / 2) * -1;
        vector<float> x(ysize, 0);
        int greaterZeroCount = 0;

        for(int i = 0; i < ysize; i++){
            x[i] = xinit + i;
            if (yobs[i] > ymin) greaterZeroCount++;
            yobs[i] = (yobs[i] - ymin) / (ymax - ymin);
            if(yobs[i] < 0) yobs[i] = 0;
        }

        bool converged = false;
        int ittr = 0;

        if (greaterZeroCount <= 3 ){
            pair<float, float> res = make_pair(FLT_MAX,
                                               FLT_MAX);
            return res;
        }
        while (!converged ) {
            if ( ittr++ > 20 ) break;
            float Rsqr = 0;
            for(int i = 0; i < ysize; i++ ){
                Rsqr += SQUARE(exp(-0.5*SQUARE(x[i]/s)) - yobs[i]);
            }

            if ( Rsqr < minR || ittr == 0 ) { minR = Rsqr; min_s = s; }
            else if ( Rsqr > minR ) break;
            else if ( Rsqr - minR == 0 ) break;
            s /= 1.25;
        }
        pair<float, float> res = make_pair(min_s, minR/(ysize*ysize));
        return res;
    }

    inline unsigned long factorial(int n)
    {
        long p = 1;
        while(n > 1)
            p *= n--;
        return p;
    }

    long long nchoosek(int n, int k)
    {
        int n_k = n - k;
        if (k < n_k){
            k = n_k;
            n_k = n - k;
        }
        long long  nchsk = 1;
        for ( int i = 1; i <= n_k; i++)
        {
            nchsk *= (++k);
            nchsk /= i;
        }
        return nchsk;
    }

    string cleanFilename(const string& filename)
    {
        string outstring = filename;
        std::string::size_type pos =outstring.find_last_of("/");

        if (pos != std::string::npos) {
            outstring=outstring.substr(pos+1, outstring.length());
        }

        pos=outstring.find_last_of("\\");
        if (pos != std::string::npos) {
            outstring=outstring.substr(pos+1, outstring.length());
        }

        pos=outstring.find_last_of(".");
        if (pos != std::string::npos) {
            outstring=outstring.substr(0, pos);
        }
        return outstring;
    }

    std::vector<double> naturalAbundanceCorrection(
        int nC,
        std::vector<double>& M,
        std::map<unsigned int, string> carbonIsotopeSpecies)
    {
        /* carbonIsotopeSpecies = {
         *   {0, C12 PARENT},
         *   {1, C13-Label-1},
         *   {2, C13-Label-2},
         *   {3, C13-Label-3},
         * }
         */

        // inputs
        int n = nC;          // number of carbonms
        int nr_13C = n + 1;  // number of labeled carbons

        // uncorrected values
        vector<double> C(nr_13C, 0);  // output vector with corrected values
        vector<double> COriginal(carbonIsotopeSpecies.size(),
                                 0);  // output vector with corrected values
        unsigned int carbonIsotopicSpeciesInt = 0;

        for (int k = 0; k < nr_13C; k++) {
            double contamination = 0;

            for (int i = 0; i < k; i++) {
                contamination += pow(0.011, k - i) * pow(0.989, n - k)
                                 * nchoosek(n - i, k - i) * C[i];
            }

            C[k] = (M[k] - contamination) / pow(0.989, n - k);
            if (C[k] < 1e-4)
                C[k] = 0;

            if (carbonIsotopeSpecies.find(k) != carbonIsotopeSpecies.end()) {
                COriginal[carbonIsotopicSpeciesInt++] = C[k];
            }
        }
        return COriginal;
    }

    double beta(double x, double y)
    {
        if (x >= 2 && y >= 2 ) {
            //approximation
            double result =  sqrt(2*M_PI) * pow(x, x - 0.5) * pow(y, y - 0.5)/
                             pow(x + y,(x + y - 0.5));
            return result;
        }
        //integral form
        double dt = 0.01;
        double sum = 0;
        for(double t = 0.0001; t <= 0.9999; t += dt) {
            sum += pow(t,(x - 1)) * pow((1 - t), (y - 1)) * dt;
        }
        return sum;
    }

    double gamma(double z)
    {
        //integral form
        double dt = 0.0001;
        double sum = 0;
        for(double t = 0.000001; t <= 10; t += dt) {
            sum += pow(t, z - 1) * exp(-t) * dt;
        }
        return sum;
    }

    double betaPDF(double x, double a, double b)
    {
        return pow(x, a - 1) * pow(1 - x, b - 1) / beta(a, b);
    }

    double pertPDF(double x, double min, double mode, double max )
    {
        double a = 6 * (mode - min)/(max - min);
        double b = 6 * (max - mode)/(max - min);
        return pow(x - min,a - 1) * pow(max - x,b - 1)/(beta(a, b)*
                                              pow(max - min,a + b - 1));
    }


    void tridiagonal ( int n, float *c, float *a, float *b, float *r )
    {
        int i;

        for ( i = 0; i < n-1; i++ ) {
            b[i] /= a[i];
            a[i+1] -= c[i]*b[i];
        }

        r[0] /= a[0];
        for ( i = 1; i < n; i++ )
            r[i] = ( r[i] - c[i-1] * r[i-1] ) / a[i];

        for ( i = n-2; i >= 0; i-- )
            r[i] -= r[i + 1] * b[i];
    }


    void cubic_nak ( int num, float *interpolatingPoints, float *functionValues,
                   float *linearCoeff, float *quadraticCoeff, float *cubicCoeff)
    {
        float *h,
              *dl,
              *dd,
              *du;
        int i;

        h  = new float [num];
        dl = new float [num];
        dd = new float [num];
        du = new float [num];

        for ( i = 0; i < num-1; i++ )
            h[i] = interpolatingPoints[i + 1] - interpolatingPoints[i];
        for ( i = 0; i < num-3; i++ )
            dl[i] = du[i] = h[i + 1];

        for ( i = 0; i < num-2; i++ ) {
            dd[i] = 2.0 * ( h[i] + h[i + 1] );
            quadraticCoeff[i]  = ( 3.0 / h[i + 1] ) * ( functionValues[i + 2] -
                                                    functionValues[i + 1] ) -
                ( 3.0 / h[i] ) * ( functionValues[i + 1] - functionValues[i] );
        }
        dd[0] += ( h[0] + h[0]*h[0] / h[1] );
        dd[num - 3] += ( h[num - 2] + h[num - 2]*h[num - 2] / h[num - 3] );
        du[0] -= ( h[0]*h[0] / h[1] );
        dl[num - 4] -= ( h[num - 2]*h[num - 2] / h[num - 3] );

        tridiagonal ( num - 2, dl, dd, du, quadraticCoeff );

        for (i = num - 3; i >= 0; i--)
            quadraticCoeff[i + 1] = quadraticCoeff[i];
        quadraticCoeff[0] = (1.0 + h[0] / h[1]) * quadraticCoeff[1]
                            - h[0] / h[1] * quadraticCoeff[2];
        quadraticCoeff[num - 1] =
            (1.0 + h[num - 2] / h[num - 3]) * quadraticCoeff[num - 2]
            - h[num - 2] / h[num - 3] * quadraticCoeff[num - 3];
        for (i = 0; i < num - 1; i++) {
            cubicCoeff[i] =
                (quadraticCoeff[i + 1] - quadraticCoeff[i]) / (3.0 * h[i]);
            linearCoeff[i] =
                (functionValues[i + 1] - functionValues[i]) / h[i]
                - h[i] * (quadraticCoeff[i + 1] + 2.0 * quadraticCoeff[i])
                      / 3.0;
        }

        delete [] h;
        delete [] du;
        delete [] dd;
        delete [] dl;
    }

    float spline_eval ( int n, float *x, float *f, float *b, float *c,
            float *d, float t )
    {
        int i=1;
        int found=0;

        while ( !found && ( i < n-1 ) ) {
            if ( t < x[i] )
                found = 1;
            else
                i++;
        }

        t = f[i-1] + ( t - x[i-1] ) * ( b[i-1] + ( t - x[i-1] ) *
                                        ( c[i-1] + (t - x[i-1] ) * d[i-1] ) );
        return ( t );
    }

    std::string decompressString(const std::string& str)
    {
        string outstring;
#ifdef ZLIB
        stringstream compressed(str);
        stringstream decompressed;
        boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
        in.push(boost::iostreams::zlib_decompressor());
        in.push(compressed);
        boost::iostreams::copy(in, decompressed);
        outstring = decompressed.str();
#endif
        return outstring;
    }

    std::string compressString(const std::string& uncompressedString) {
        string outstring;
#ifdef ZLIB
        stringstream uncompressed(uncompressedString);
        stringstream compressed;
        boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
        in.push(boost::iostreams::zlib_compressor());
        in.push(uncompressed);
        boost::iostreams::copy(in, compressed);
        outstring = compressed.str();
#endif
        return outstring;
    }

    bool gzipInflate( const std::string& compressedBytes,
                     std::string& uncompressedBytes )
    {
#ifdef ZLIB
        if ( compressedBytes.size() == 0 ) {
            uncompressedBytes = compressedBytes ;
            return true ;
        }

        uncompressedBytes.clear() ;

        unsigned full_length = compressedBytes.size() ;
        unsigned half_length = compressedBytes.size() / 2;

        unsigned uncompLength = full_length ;
        char* uncomp = (char*) calloc( sizeof(char), uncompLength );

        z_stream strm;
        strm.next_in = (Bytef *) compressedBytes.c_str();
        strm.avail_in = compressedBytes.size() ;
        strm.total_out = 0;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;

        bool done = false ;

        if (inflateInit2(&strm, (16+MAX_WBITS)) != Z_OK) {
            free( uncomp );
            return false;
        }

        while (!done) {
            // If our output buffer is too small
            if (strm.total_out >= uncompLength ) {
                // Increase size of output buffer
                char* uncomp2 = (char*) calloc( sizeof(char),
                                              uncompLength + half_length );
                memcpy( uncomp2, uncomp, uncompLength );
                uncompLength += half_length ;
                free( uncomp );
                uncomp = uncomp2 ;
            }

            strm.next_out = (Bytef *) (uncomp + strm.total_out);
            strm.avail_out = uncompLength - strm.total_out;

            // Inflate another chunk.
            int err = inflate (&strm, Z_SYNC_FLUSH);
            if (err == Z_STREAM_END) done = true;
            else if (err != Z_OK)  {
                break;
            }
        }

        if (inflateEnd (&strm) != Z_OK) {
            free( uncomp );
            return false;
        }

        for ( size_t i = 0; i<strm.total_out; ++i ) {
            uncompressedBytes += uncomp[ i ];
        }
        free( uncomp );
#endif
        return true ;
    }


    bool compareStringsWithNumbers(const std::string& a, const std::string& b ) {
        if (a.empty())
            return true;
        if (b.empty())
            return false;
        if (std::isdigit(a[0]) && !std::isdigit(b[0]))
            return true;
        if (!std::isdigit(a[0]) && std::isdigit(b[0]))
            return false;
        if (!std::isdigit(a[0]) && !std::isdigit(b[0])) {
            if (std::toupper(a[0]) == std::toupper(b[0]))
                return compareStringsWithNumbers(a.substr(1), b.substr(1));
            return (std::toupper(a[0]) < std::toupper(b[0]));
        }

        // Both strings begin with digit --> parse both numbers
        std::istringstream issa(a);
        std::istringstream issb(b);
        int ia, ib;
        issa >> ia;
        issb >> ib;
        if (ia != ib)
            return ia < ib;

        // Numbers are the same --> remove numbers and recurse
        std::string anew, bnew;
        std::getline(issa, anew);
        std::getline(issb, bnew);
        return (compareStringsWithNumbers(anew, bnew));
    }

    double besseli0(const double x)
    {
        double y = 1.0;
        double s = 1.0;
        double n = 1.0;
        double x2 = x * x;

        while (s > y * 1.0e-9) {
            s *= x2 / 4.0 / (n * n);
            y += s;
            n += 1;
        }
        return y;
    }

    std::vector<double> kaiser(const size_t N, const double beta)
    {
        std::vector<double> h(N);
        double bb = besseli0(beta);
        for (size_t i = 0; i < N; i++) {
            h[i] = besseli0(beta * sqrt(4.0 * i * (N - 1 - i)) / (N - 1)) / bb;
        }
        return h;
    }

    double sinc(const double x)
    {
        // from : http://mathworld.wolfram.com/SincFunction.html
        if (fabs(x) < 0.01)
            return cos(M_PI * x / 2.0)
                   * cos(M_PI * x / 4.0)
                   * cos(M_PI * x / 8.0);

        // compute sinc(x) = sin(π•x) / (π•x)
        return sin(M_PI * x) / (M_PI * x);
    }

    std::vector<double> firDesignKaiser(const size_t n,
                                        const double fc,
                                        const double beta,
                                        const double scale)
    {
        // validate inputs
        if (fc < 0.0 || fc > 0.5) {
            std::cerr << "Error: cutoff frequency "
                      << fc
                      << " out of range; should be within (0, 0.5)"
                      << std::endl;
            return {};
        } else if (n == 0) {
            std::cerr << "Error: filter length must be greater than zero"
                      << std::endl;
            return {};
        }

        // kaiser window
        auto kw = kaiser(n, beta);

        std::vector<double> filter;
        auto t = 0.0;
        auto h1 = 0.0;
        auto h2 = 0.0;
        for (size_t i = 0; i < n; i++) {
            t = double(i) - double((n - 1) / 2.0);

            // sinc prototype
            h1 = sinc(fc * t);
            h2 = fc * kw.at(i);

            // composite
            filter.push_back(scale * h1 * h2);
        }
        return filter;
    }

    std::vector<double> computeFilterCoefficients(const int interpRate,
                                                  const int decimRate)
    {
        int maxRate = std::max(interpRate, decimRate);

        // reasonable half-length for our sinc-like function
        int halfLen = 10 * maxRate;
        int numTaps = 2 * halfLen + 1;

        // cutoff freq of FIR filter (rel. to Nyquist)
        double fc = 1.0 / maxRate;

        // beta for Kaiser window, 5.0 seems to work for all EICs that we tested
        double beta = 5.0;

        return firDesignKaiser(numTaps, fc, beta, interpRate);
    }

    int approximateResamplingFactor(const size_t dataSize, int lowerSizeLimit)
    {
        int resamplingFactor = static_cast<int>(dataSize / lowerSizeLimit) - 1;
        if (resamplingFactor < 1)
            resamplingFactor = 1;

        return resamplingFactor;
    }

    std::vector<double> resample(const std::vector<double>& inputData,
                                 const int interpRate,
                                 const int decimRate)
    {
        // ignore resampling for rates less than or equal to 1
        if (interpRate <= 1 && decimRate <= 1)
            return inputData;

        auto filterTaps = computeFilterCoefficients(interpRate, decimRate);
        NimbleDSP::RealFirFilter<double> filter(filterTaps);
        filter.filtOperation = NimbleDSP::ONE_SHOT_TRIM_TAILS;

        NimbleDSP::RealVector<double> buf(inputData);
        buf = filter.resample(buf, interpRate, decimRate);
        return buf.vec;
    }

    chrono::time_point<chrono::high_resolution_clock> startTimer()
    {
        return chrono::high_resolution_clock::now();
    }

    void stopTimer(chrono::time_point<chrono::high_resolution_clock>& clock,
                   string name)
    {
        auto now = chrono::high_resolution_clock::now();
        auto diff = now - clock;
        cerr << "RUNTIME OF " << name << ": "
             << chrono::duration_cast<chrono::milliseconds>(diff).count()
             << " ms\n";
    }

    unsigned long long availableSystemMemory()
    {
#ifdef UNIX
        long long pages = sysconf(_SC_PHYS_PAGES);
        long long page_size = sysconf(_SC_PAGE_SIZE);
        return pages * page_size;
#endif
#ifdef WIN32
        MEMORYSTATUSEX status;
        status.dwLength = sizeof(status);
        GlobalMemoryStatusEx(&status);
        return status.ullTotalPhys;
#endif
    }

    int numSystemCpus()
    {
        int nCpu = thread::hardware_concurrency();
        return nCpu;
    }

    int compareDates(string date1, string date2) 
    {
        auto splitDate1 = split(date1, "-");
        auto splitDate2 = split(date2, "-");
        auto year1 = string2integer(splitDate1[0]);
        auto year2 = string2integer(splitDate2[0]);
        auto month1 = string2integer(splitDate1[1]);
        auto month2 = string2integer(splitDate2[1]);
        auto day1 = string2integer(splitDate1[2]);
        auto day2 = string2integer(splitDate2[2]);
        
        if (year1 < year2) {
            return -1;
        } else if (year1 == year2) {
            if (month1 < month2) {
                return -1;
            } else if (month1 == month2) {
                if (day1 < day2)
                    return -1;
                else if (day1 == day2)
                    return 0;
                else
                    return 1;
            } else {
                return 1;
            }
        } else {
            return 1;
        } 
        return 1;
    }

    bool compareTime(string time1, string time2) 
    {
        auto splitTime1 = split(time1, ":");
        auto splitTime2 = split(time2, ":");
        auto hour1 = string2integer(splitTime1[0]);
        auto hour2 = string2integer(splitTime2[0]);
        auto minutes1 = string2integer(splitTime1[1]);
        auto minutes2 = string2integer(splitTime2[1]);
        auto seconds1 = string2integer(splitTime1[2]);
        auto seconds2 = string2integer(splitTime2[2]);
        
        if (hour1 < hour2) {
            return true;
        } else if (hour1 == hour2) {
            if (minutes1 < minutes2) {
                return true;
            } else if (minutes1 == minutes2) {
                if (seconds1 < seconds2)
                    return true;
                else
                    return false;
            } else {
                return false;
            }
        } else {
            return false;
        }
        return false;
    }

} //namespace end

//////////////////////////////////////TestCases/////////////////////////////

TEST_SUITE("Testing mzUtils functions")
{
    TEST_CASE("Testing random functions")
    {
        SUBCASE("Testing Random Integer")
        {
            int res = mzUtils::randInt(0, 100);
            REQUIRE((res <= 100 && res >= 0) == true);
            res = mzUtils::randInt(INT_MIN, INT_MAX);
            REQUIRE((res >= INT_MIN && res <= INT_MAX) == true) ;
        }

        SUBCASE("Testing Random Long Integer")
        {
            long int res = mzUtils::randInt(5000,10000);
            REQUIRE((res <= 100000 && res >= 5000) == true);
            res = mzUtils::randLong(LONG_MIN, LONG_MAX);
            REQUIRE((res >= LONG_MIN && res <= LONG_MAX) == true) ;
        }

        SUBCASE("Testing Random Float")
        {
            float res = mzUtils::randInt(0.0,100.0);
            REQUIRE((res <= 100.0 && res >= 0.0) == true);
            res = mzUtils::randFloat(FLT_MIN, FLT_MAX);
            REQUIRE((res >= FLT_MIN && res <= FLT_MAX) == true) ;
        }

        SUBCASE("Testing Random Double")
        {
            double res = mzUtils::randInt(5000.0,10000.0);
            REQUIRE((res <= 100000.0 && res >= 5000.0) == true);
            res = mzUtils::randDouble(FLT_MIN, FLT_MAX);
            REQUIRE((res >= FLT_MIN && res <= FLT_MAX) == true) ;
        }
    }

    TEST_CASE("Testing split function")
    {
        string str = "Hello,this,is,to,test,function";
        string sep = ",";
        vector<string> result;
        result = mzUtils::split(str, sep);
        REQUIRE(result.size() == 6);
        REQUIRE(result[0] == "Hello");
        REQUIRE(result[1] == "this");
        REQUIRE(result[2] == "is");
        REQUIRE(result[3] == "to");
        REQUIRE(result[4] == "test");
        REQUIRE(result[5] == "function");
    }

    TEST_CASE("Testing join function")
    {
        vector<string> words = {"Hello!", "This", "is", "a", "test."};
        string sep = " ";
        string result = mzUtils::join(words, sep);
        REQUIRE(result == "Hello! This is a test.");
    }

    TEST_CASE("Testing converting to Lower case")
    {
        string str = "TESTING";
        mzUtils::makeLowerCase(str);
        REQUIRE(str == "testing");
    }

    TEST_CASE("Removing Special Characters")
    {
        string str1 = "\"Doctest\"";
        string str2 = "\directory";
        vector<string> fields;
        fields.push_back(str1);
        fields.push_back(str2);
        mzUtils::removeSpecialCharFromStartEnd(fields);
        REQUIRE(fields[0] == "Doctest");
        REQUIRE(fields[1] == "directory");
    }

    TEST_CASE("Testing Compare strings")
    {
        string str = "mzCSVFile";
        REQUIRE(mzUtils::contains(str, "mzCSV") == true);
    }

    TEST_CASE("Testing Smooth Average")
    {
        float *input = new float[10];
        input[0] = 10.002;
        input[1] = 15.001;
        input[2] = 22.002;
        input[3] = 42.229;
        input[4] = 28.992;
        input[5] = 11.09;
        input[6] = 12.091;
        input[7] = 33.082;
        input[8] = 12.234;
        input[9] = 43.998;

        float *result = new float[10];
        mzUtils::smoothAverage(input, result, 5, 10);

        REQUIRE(doctest::Approx(result[0]) == 9.401);
        REQUIRE(doctest::Approx(result[1]) == 17.8468);
        REQUIRE(doctest::Approx(result[2]) == 23.6452);
        REQUIRE(doctest::Approx(result[3]) == 23.8628);
        REQUIRE(doctest::Approx(result[4]) == 23.2808);
        REQUIRE(doctest::Approx(result[5]) == 25.4968);
        REQUIRE(doctest::Approx(result[6]) == 19.4978);
        REQUIRE(doctest::Approx(result[7]) == 22.499);
        REQUIRE(doctest::Approx(result[8]) == 20.281);
        REQUIRE(doctest::Approx(result[9]) == 17.8628);
    }


    TEST_CASE("Testing Gaussian 1D Smoothing")
    {
        float *input = new float[10];
        input[0] = 10.002;
        input[1] = 15.001;
        input[2] = 22.002;
        input[3] = 42.229;
        input[4] = 28.992;
        input[5] = 11.09;
        input[6] = 12.091;
        input[7] = 33.082;
        input[8] = 12.234;
        input[9] = 43.998;
        mzUtils::gaussian1d_smoothing(10, 5, input);
        REQUIRE(doctest::Approx(input[0]) == 10.9483);
        REQUIRE(doctest::Approx(input[1]) == 16.1078);
        REQUIRE(doctest::Approx(input[2]) == 20.584);
        REQUIRE(doctest::Approx(input[3]) == 23.111);
        REQUIRE(doctest::Approx(input[4]) == 23.4083);
        REQUIRE(doctest::Approx(input[5]) == 22.4539);
        REQUIRE(doctest::Approx(input[6]) == 21.5049);
        REQUIRE(doctest::Approx(input[7]) == 20.8023);
        REQUIRE(doctest::Approx(input[8]) == 19.4174);
        REQUIRE(doctest::Approx(input[9]) == 16.3895);
    }

    TEST_CASE("Testing Medians")
    {
        vector<float> input;
        input.push_back(10.002);
        input.push_back(15.001);
        input.push_back(22.002);
        input.push_back(42.229);
        input.push_back(28.992);
        input.push_back(11.09);
        input.push_back(12.091);
        input.push_back(33.082);
        input.push_back(12.234);
        input.push_back(43.998);
        float res = mzUtils::median(input);
        REQUIRE(doctest::Approx(res) == 18.5015f);

    }

    TEST_CASE("Testinng Convert Datatypes")
    {
        string str = mzUtils::integer2string(29);
        REQUIRE(str == "29");

        float res = mzUtils::string2float("30.0");
        REQUIRE(doctest::Approx(res) == 30.0);

        int result = mzUtils::string2integer("98");
        REQUIRE(result == 98);
    }

    TEST_CASE("Testing MassCutoffDist")
    {
        MassCutoff* massCutoff = new MassCutoff();
        massCutoff->setMassCutoffAndType(0.3, "ppm");

        SUBCASE("Testing float parameters")
        {
            float res = mzUtils::massCutoffDist(10.3, 10.8, massCutoff);
            REQUIRE(doctest::Approx(res) == 48543.7);
        }

        SUBCASE("Testing Double parameters")
        {
            float res = mzUtils::massCutoffDist(100.003, 150.8027, massCutoff);
            REQUIRE(doctest::Approx(res) == 507982);
        }

        SUBCASE("Testing withinXMasscutOff")
        {
            bool res = mzUtils::withinXMassCutoff(10.3, 10.8, massCutoff);
            REQUIRE(res == false);
        }
    }

    TEST_CASE("Testing ppmDist functions")
    {
        SUBCASE("Testing Float parameters")
        {
            float res = mzUtils::ppmDist(10.3, 10.8);
            REQUIRE(doctest::Approx(res) == 48543.7);
        }

        SUBCASE("Testing Double parameters")
        {
            float res = mzUtils::ppmDist(100.003, 150.8027);
            REQUIRE(doctest::Approx(res) == 507982);
        }

        SUBCASE("Testing ppmRound")
        {
            float res = mzUtils::ppmround(10.3,100);
            REQUIRE(doctest::Approx(res) == 10.3);
        }
    }

    TEST_CASE("Testing QuantileDistribution")
    {
        vector<float> input;
        input.push_back(10.002);
        input.push_back(15.001);
        input.push_back(22.002);
        input.push_back(42.229);
        input.push_back(28.992);
        input.push_back(11.09);
        input.push_back(12.091);
        input.push_back(33.082);
        input.push_back(12.234);
        input.push_back(43.998);

        vector<float> res;
        res = mzUtils::quantileDistribution(input);
        for( int i = 0; i < 10; i++ )
            REQUIRE(doctest::Approx(res[i]) == 10.002);
        for( int i = 10; i < 20; i++ )
            REQUIRE(doctest::Approx(res[i]) == 11.09);
        for( int i = 20; i < 30; i++ )
            REQUIRE(doctest::Approx(res[i]) == 12.091);
        for( int i = 30; i < 40; i++ )
            REQUIRE(doctest::Approx(res[i]) == 12.234);
        for( int i = 40; i < 50; i++ )
            REQUIRE(doctest::Approx(res[i]) == 15.001);
        for( int i = 50; i < 60; i++ )
            REQUIRE(doctest::Approx(res[i]) == 22.002);
        for( int i = 60; i < 70; i++ )
            REQUIRE(doctest::Approx(res[i]) == 28.992);
        for( int i = 70; i < 80; i++ )
            REQUIRE(doctest::Approx(res[i]) == 33.082);
        for( int i = 80; i < 90; i++ )
            REQUIRE(doctest::Approx(res[i]) == 42.229);
        for( int i = 90; i < 100; i++ )
            REQUIRE(doctest::Approx(res[i]) == 43.998);
        REQUIRE(res[100] == 0);
    }


    TEST_CASE("Testing t_test")
    {
        StatisticsVector<float> groupA;
        groupA.push_back(10.002);
        groupA.push_back(15.001);
        groupA.push_back(22.002);
        groupA.push_back(42.229);
        groupA.push_back(28.992);
        groupA.push_back(11.09);
        groupA.push_back(12.091);
        groupA.push_back(33.082);
        groupA.push_back(12.234);
        groupA.push_back(43.998);

        StatisticsVector<float> groupB;
        groupB.push_back(13.002);
        groupB.push_back(11.001);
        groupB.push_back(20.002);
        groupB.push_back(22.229);
        groupB.push_back(23.992);
        groupB.push_back(31.09);
        groupB.push_back(22.091);
        groupB.push_back(43.082);
        groupB.push_back(82.234);
        groupB.push_back(20.998);

        float res = mzUtils::ttest(groupA, groupB);
        REQUIRE( doctest::Approx(res) == -0.758955);
    }

    TEST_CASE("Testing fileExists")
    {
        REQUIRE((mzUtils::fileExists("testCharge.xml")) == true);
        REQUIRE((mzUtils::fileExists("hello.cpp")) == false);
    }

    TEST_CASE("Testing AlmostEquals")
    {
        REQUIRE((mzUtils::almostEqual(33.082, 43.085)) == false);
    }

    TEST_CASE("Testing correlation")
    {
        StatisticsVector<float> groupA;
        groupA.push_back(10.002);
        groupA.push_back(15.001);
        groupA.push_back(22.002);
        groupA.push_back(42.229);
        groupA.push_back(28.992);
        groupA.push_back(11.09);
        groupA.push_back(12.091);
        groupA.push_back(33.082);
        groupA.push_back(12.234);
        groupA.push_back(43.998);

        StatisticsVector<float> groupB;
        groupB.push_back(13.002);
        groupB.push_back(11.001);
        groupB.push_back(20.002);
        groupB.push_back(22.229);
        groupB.push_back(23.992);
        groupB.push_back(31.09);
        groupB.push_back(22.091);
        groupB.push_back(43.082);
        groupB.push_back(82.234);
        groupB.push_back(20.998);

        float res = mzUtils::correlation(groupA, groupB);
        REQUIRE(doctest::Approx(res) == -0.141492);
    }

    TEST_CASE("Testing combination and factorial")
    {
        SUBCASE("Testing Factorial")
        {
            REQUIRE(mzUtils::factorial(4) == 24);
        }
        SUBCASE("Testing Combination")
        {
            REQUIRE((mzUtils::nchoosek(5,2)) == 10);
        }
    }

    TEST_CASE("Testing cleaning of filename")
    {
        string str ="tests/test-libmaven/test_jsonReports.csv";
        string res = mzUtils::cleanFilename(str);
        REQUIRE(res == "test_jsonReports");
    }

    TEST_CASE("Testing beta and gama functions")
    {
        float res = mzUtils::beta(0.78, 1.87);
        REQUIRE(doctest::Approx(res) == 0.815535);

        res = mzUtils::gamma(0.67);
        REQUIRE(doctest::Approx(res) == 1.35569);

        res = mzUtils::betaPDF(0.78, 1.87, 0.67);
        REQUIRE(doctest::Approx(res) == 1.48414);
    }

    TEST_CASE("Testing cubic_nak and spline")
    {
        float *input = new float[5];
        input[0] = 10.002;
        input[1] = 15.001;
        input[2] = 22.002;
        input[3] = 42.229;
        input[4] = 28.992;

        float *functionValues = new float[5];
        functionValues[0] = 12.65;
        functionValues[1] = 10.5;
        functionValues[2] = 15.32;
        functionValues[3] = 18.65;
        functionValues[4] = 2.65;

        float *linearCoeff = new float[5];
        linearCoeff[0] = 0;
        linearCoeff[1] = 0;
        linearCoeff[2] = 0;
        linearCoeff[3] = 0;
        linearCoeff[4] = 0;

        float *quadraticCoef = new float[5];
        quadraticCoef[0] =0;
        quadraticCoef[1] =0;
        quadraticCoef[2] =0;
        quadraticCoef[3] =0;
        quadraticCoef[4] =0;

        float *cubicCoef = new float[5];
        cubicCoef[0] = 0;
        cubicCoef[1] = 0;
        cubicCoef[2] = 0;
        cubicCoef[3] = 0;
        cubicCoef[4] = 0;

        mzUtils::cubic_nak(5, input, functionValues,
                           linearCoeff, quadraticCoef, cubicCoef);
        REQUIRE(doctest::Approx(linearCoeff[0]) == -2.17844);
        REQUIRE(doctest::Approx(linearCoeff[1]) == 0.78405);
        REQUIRE(doctest::Approx(linearCoeff[2]) == -0.45489);
        REQUIRE(doctest::Approx(linearCoeff[3]) == 7.73426);
        REQUIRE(doctest::Approx(linearCoeff[4]) == 0);

        REQUIRE(doctest::Approx(quadraticCoef[0]) == 0.456606);
        REQUIRE(doctest::Approx(quadraticCoef[1]) ==  0.136011);
        REQUIRE(doctest::Approx(quadraticCoef[2]) == -0.312977);
        REQUIRE(doctest::Approx(quadraticCoef[3]) ==  0.717839);
        REQUIRE(doctest::Approx(quadraticCoef[4]) ==  0.0432502);

        REQUIRE(doctest::Approx(cubicCoef[0]) == -0.0213773);
        REQUIRE(doctest::Approx(cubicCoef[1]) == -0.0213773);
        REQUIRE(doctest::Approx(cubicCoef[2]) == 0.0169875);
        REQUIRE(doctest::Approx(cubicCoef[3]) == 0.0169875);
        REQUIRE(doctest::Approx(cubicCoef[4]) == 0);

        mzUtils::spline_eval(5,input, functionValues,
                             linearCoeff, quadraticCoef, cubicCoef, 0.34f);
        REQUIRE(doctest::Approx(linearCoeff[0]) == -2.17844);
        REQUIRE(doctest::Approx(linearCoeff[1]) == 0.78405);
        REQUIRE(doctest::Approx(linearCoeff[2]) == -0.45489);
        REQUIRE(doctest::Approx(linearCoeff[3]) == 7.73426);
        REQUIRE(doctest::Approx(linearCoeff[4]) == 0);

        REQUIRE(doctest::Approx(quadraticCoef[0]) == 0.456606);
        REQUIRE(doctest::Approx(quadraticCoef[1]) ==  0.136011);
        REQUIRE(doctest::Approx(quadraticCoef[2]) == -0.312977);
        REQUIRE(doctest::Approx(quadraticCoef[3]) ==  0.717839);
        REQUIRE(doctest::Approx(quadraticCoef[4]) ==  0.0432502);

        REQUIRE(doctest::Approx(cubicCoef[0]) == -0.0213773);
        REQUIRE(doctest::Approx(cubicCoef[1]) == -0.0213773);
        REQUIRE(doctest::Approx(cubicCoef[2]) == 0.0169875);
        REQUIRE(doctest::Approx(cubicCoef[3]) == 0.0169875);
        REQUIRE(doctest::Approx(cubicCoef[4]) == 0);
    }

    TEST_CASE("Tesrting Decompress String")
    {
        string destStr = "TestingDoneSuccessfully";
        string str = "Testing";
        bool res = mzUtils::gzipInflate(destStr, str);
        REQUIRE(res == true);
    }

    TEST_CASE("Testing comparing numbers in string format")
    {
        string str1 = "7648";
        string str2 = "7648";
        bool res = mzUtils::compareStringsWithNumbers(str1, str2);
        REQUIRE(res == true);
    }

    TEST_CASE("Testing besseli0")
    {
        double res = mzUtils::besseli0(0.34);
        REQUIRE(doctest::Approx(res) == 1.02911);
    }

    TEST_CASE("Testing Kaiser")
    {
        vector<double> vect = mzUtils::kaiser(3, 2.3);
        REQUIRE(doctest::Approx(vect[0]) == 0.353406);
        REQUIRE(doctest::Approx(vect[1]) == 1);
        REQUIRE(doctest::Approx(vect[2]) == 0.353406);
    }

    TEST_CASE("Testing sinc")
    {
        float res = mzUtils::sinc(2.3);
        REQUIRE(doctest::Approx(res) == 0.111964);
    }

    TEST_CASE("Testing firDesignKaiser")
    {
        vector<double> vect = mzUtils::firDesignKaiser(3, 0.1);
        REQUIRE(doctest::Approx(vect[0]) == 0.003611);
        REQUIRE(doctest::Approx(vect[1]) ==  0.1);
        REQUIRE(doctest::Approx(vect[2]) == 0.003611);
    }

    TEST_CASE("Testing Filter Coefficient")
    {
        vector<double> vect = mzUtils::computeFilterCoefficients(1, 2);
        REQUIRE(doctest::Approx(vect[0]) == -7.15527e-19);
        REQUIRE(doctest::Approx(vect[1]) == -0.00105091);
        REQUIRE(doctest::Approx(vect[2]) == 1.85334e-18);
        REQUIRE(doctest::Approx(vect[3]) == 0.00250767);
        REQUIRE(doctest::Approx(vect[4]) == -3.49234e-18);
        REQUIRE(doctest::Approx(vect[5]) == -0.0048923);
        REQUIRE(doctest::Approx(vect[6]) == 5.60355e-18);
        REQUIRE(doctest::Approx(vect[7]) == 0.00855213);
        REQUIRE(doctest::Approx(vect[8]) == -8.08683e-18);
        REQUIRE(doctest::Approx(vect[9]) == -0.0139827);
        REQUIRE(doctest::Approx(vect[10]) == 1.07756e-17);
        REQUIRE(doctest::Approx(vect[11]) == 0.0220117);
        REQUIRE(doctest::Approx(vect[12]) == -1.34527e-17);
        REQUIRE(doctest::Approx(vect[13]) == -0.0343224);
        REQUIRE(doctest::Approx(vect[14]) == 1.58764e-17);
        REQUIRE(doctest::Approx(vect[15]) == 0.0552597);
        REQUIRE(doctest::Approx(vect[16]) == -1.7811e-17);
        REQUIRE(doctest::Approx(vect[17]) == -0.100878);
        REQUIRE(doctest::Approx(vect[18]) == 1.90594e-17);
        REQUIRE(doctest::Approx(vect[19]) == 0.316537);
        REQUIRE(doctest::Approx(vect[20]) == 0.5);
        REQUIRE(doctest::Approx(vect[21]) == 0.316537);
        REQUIRE(doctest::Approx(vect[22]) == 1.90594e-17);
        REQUIRE(doctest::Approx(vect[23]) == -0.100878);
        REQUIRE(doctest::Approx(vect[24]) == -1.7811e-17);
        REQUIRE(doctest::Approx(vect[25]) == 0.0552597);
        REQUIRE(doctest::Approx(vect[26]) == 1.58764e-17);
        REQUIRE(doctest::Approx(vect[27]) == -0.0343224);
        REQUIRE(doctest::Approx(vect[28]) == -1.34527e-17);
        REQUIRE(doctest::Approx(vect[29]) == 0.0220117);
        REQUIRE(doctest::Approx(vect[30]) == 1.07756e-17);
        REQUIRE(doctest::Approx(vect[31]) == -0.0139827);
        REQUIRE(doctest::Approx(vect[32]) == -8.08683e-18);
        REQUIRE(doctest::Approx(vect[33]) == 0.00855213);
        REQUIRE(doctest::Approx(vect[34]) == 5.60355e-18);

    }

    TEST_CASE("Testing Resampling Factor")
    {
        int res = mzUtils::approximateResamplingFactor(5, 3);
        REQUIRE(res == 1);
    }

    TEST_CASE("Testing resample")
    {
        vector<double> input;
        input.push_back(10.002);
        input.push_back(15.001);
        input.push_back(22.002);
        vector<double> vect = mzUtils::resample(input, 1, 1);
        REQUIRE(doctest::Approx(vect[0]) == 10.002);
        REQUIRE(doctest::Approx(vect[1]) ==  15.001);
        REQUIRE(doctest::Approx(vect[2]) == 22.002);
    }

    TEST_CASE("Testing GaussFit")
    {
        vector<float> input;
        input.push_back(10.002);
        input.push_back(15.001);
        input.push_back(22.002);
        input.push_back(42.229);
        input.push_back(28.992);
        input.push_back(11.09);
        input.push_back(12.091);
        input.push_back(33.082);
        input.push_back(12.234);
        input.push_back(43.998);

        pair<float, float> res = mzUtils::gaussFit(input);
        REQUIRE(doctest::Approx(res.first) == 4.1943);
        REQUIRE(doctest::Approx(res.second) == 0.0455033);
    }

    TEST_CASE("Testing decompress string")
    {
        string str = "Testing if test works";
        string compress = mzUtils::compressString(str);
        string res = mzUtils::decompressString(compress);
        REQUIRE(res == str);
    }

    TEST_CASE("Testing CreateDir")
    {
        string path = "tests/path";
        int res = mzUtils::createDir(path);
        REQUIRE(res == 0);
        remove(path.c_str());
    }
}
