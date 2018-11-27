#include "testMzFit.h"


TestMzFit::TestMzFit() {

}

void TestMzFit::initTestCase() {
    // This function is being executed at the beginning of each test suite
    // That is - before other tests from this class run
    //MavenParameters* mavenparemeters = new MavenParameters();
    //mavenparemeters->printSettings();
}

void TestMzFit::cleanupTestCase() {
    // Similarly to initTestCase(), this function is executed at the end of test suite
}

void TestMzFit::init() {
    // This function is executed before each test
}

void TestMzFit::cleanup() {
    // This function is executed after each test
}


void TestMzFit::testSort_xy() {

    double* x  = new double[6];
    double* y =  new double[6];

    x[0]=(double)5/7;
    y[0]=(double)5/9;
    x[1]=(double)8/9;
    y[1]=(double)4/9;
    x[2]=(double)6/7;
    y[2]=(double)2/9;
    x[3]=(double)3/7;
    y[3]=(double)6/9;
    x[4]=(double)1/7;
    y[4]=(double)7/9;
    x[5]=(double)2/7;
    y[5]=(double)3/9;

    sort_xy(x, y, 6, 1, 0);

    QVERIFY(TestUtils::floatCompare(x[0],(double) 6/7));
    QVERIFY(TestUtils::floatCompare(y[0],(double) 2/9));
    QVERIFY(TestUtils::floatCompare(x[1],(double) 2/7));
    QVERIFY(TestUtils::floatCompare(y[1],(double) 3/9));
    QVERIFY(TestUtils::floatCompare(x[2],(double) 8/9));
    QVERIFY(TestUtils::floatCompare(y[2],(double) 4/9));
    QVERIFY(TestUtils::floatCompare(x[3],(double) 5/7));
    QVERIFY(TestUtils::floatCompare(y[3],(double) 5/9));
    QVERIFY(TestUtils::floatCompare(x[4],(double) 3/7));
    QVERIFY(TestUtils::floatCompare(y[4],(double) 6/9));
    QVERIFY(TestUtils::floatCompare(x[5],(double) 1/7));
    QVERIFY(TestUtils::floatCompare(y[5],(double) 7/9));

    delete x;
    delete y;
}



void TestMzFit::testGauss() {
    int degree=4;
    int wdim=10;
    double r[10]={0,1.13635098543487e-322,2.12953330784105e-316,1.69119330067722e-306,1.33511561495037e-306,6.89811601842991e-307,1.60219306218075e-306,1.11261501989027e-306,6.95352557462179e-308,1.06764629324507e-312};
    double w[100] = {6,3.31746031746032,2.32073570168808,1.80145732602271,1.46519470212616,1.71799194089032e-316,1.72378239025958e-316,1.71194852991039e-316,1.70926713683728e-316,1.72383693510688e-316,3.31746031746032,2.32073570168808,1.80145732602271,1.46519470212616,1.21994948446795,1.71299278705944e-316,1.72031839720345e-316,1.71930338874064e-316,1.72595944112141e-316,1.72601398596871e-316,2.32073570168808,1.80145732602271,1.46519470212616,1.21994948446795,1.02939883814978,1.72012788549042e-316,1.71435798925202e-316,1.7191769079353e-316,1.72442665186175e-316,1.72395946338705e-316,1.80145732602271,1.46519470212616,1.21994948446795,1.02939883814978,0.876055808585734,1.71278646524573e-316,1.71114458629147e-316,1.72311520401143e-316,1.71981484549721e-316,1.72039902871685e-316,1.46519470212616,1.21994948446795,1.02939883814978,0.876055808585734,0.750044551236736,1.71852316027272e-316,1.72504561730286e-316,1.72497368134483e-316,1.72608987445191e-316,1.71780142917728e-316,1.72589145768854e-316,1.71651132496286e-316,1.7172117124224e-316,1.71637535809712e-316,1.70912721744637e-316,1.71937137217351e-316,1.71565283649665e-316,1.71676270556346e-316,1.71644334152999e-316,1.71449395611776e-316,1.71539038882557e-316,1.72409384924271e-316,1.72435076337855e-316,1.71377459653742e-316,1.72141640869477e-316,1.72018243033772e-316,1.71582990962411e-316,1.71558090053861e-316,1.71590975063248e-316,1.71625282981696e-316,1.72490174538679e-316,1.71618089385892e-316,1.71492952439113e-316,1.71411293269169e-316,1.71323784361977e-316,1.71518801953704e-316,1.70900073664104e-316,1.71818245260335e-316,1.71356274118848e-316,1.71036831034873e-316,1.71844331926436e-316,1.71331768462814e-316,1.71397854683602e-316,1.71416747753899e-316,1.71304733190674e-316,1.70907267259907e-316,1.71871762451093e-316,1.71656586981016e-316,1.71403309168332e-316,1.71147501739541e-316,1.71159754567558e-316,1.71344021290831e-316,1.71215643273416e-316,1.71338566806101e-316,1.7190346170293e-316,1.70893275320817e-316,1.71134853659008e-316,1.71233350586163e-316,1.7088647697753e-316,1.71128055315721e-316};
    double wcomp[100] = {6,3.31746031746032,2.32073570168808,1.80145732602271,1.46519470212616,1.71799194089032e-316,1.72378239025958e-316,1.71194852991039e-316,1.70926713683728e-316,1.72383693510688e-316,3.31746031746032,0.486478542034097,0.518299226412101,0.469150836679741,0.409828604191842,1.71299278705944e-316,1.72031839720345e-316,1.71930338874064e-316,1.72595944112141e-316,1.72601398596871e-316,2.32073570168808,0.518299226412101,0.0153576928141729,0.0233269844017706,0.0260416321938787,1.72012788549042e-316,1.71435798925202e-316,1.7191769079353e-316,1.72442665186175e-316,1.72395946338705e-316,1.80145732602271,0.469150836679741,0.0233269844017706,0.000652130616958477,0.00135550615936962,1.71278646524573e-316,1.71114458629147e-316,1.72311520401143e-316,1.71981484549721e-316,1.72039902871685e-316,1.46519470212616,0.409828604191842,0.0260416321938788,0.0013555061593696,1.39747393326566e-05,1.71852316027272e-316,1.72504561730286e-316,1.72497368134483e-316,1.72608987445191e-316,1.71780142917728e-316,1.72589145768854e-316,1.71651132496286e-316,1.7172117124224e-316,1.71637535809712e-316,1.70912721744637e-316,1.71937137217351e-316,1.71565283649665e-316,1.71676270556346e-316,1.71644334152999e-316,1.71449395611776e-316,1.71539038882557e-316,1.72409384924271e-316,1.72435076337855e-316,1.71377459653742e-316,1.72141640869477e-316,1.72018243033772e-316,1.71582990962411e-316,1.71558090053861e-316,1.71590975063248e-316,1.71625282981696e-316,1.72490174538679e-316,1.71618089385892e-316,1.71492952439113e-316,1.71411293269169e-316,1.71323784361977e-316,1.71518801953704e-316,1.70900073664104e-316,1.71818245260335e-316,1.71356274118848e-316,1.71036831034873e-316,1.71844331926436e-316,1.71331768462814e-316,1.71397854683602e-316,1.71416747753899e-316,1.71304733190674e-316,1.70907267259907e-316,1.71871762451093e-316,1.71656586981016e-316,1.71403309168332e-316,1.71147501739541e-316,1.71159754567558e-316,1.71344021290831e-316,1.71215643273416e-316,1.71338566806101e-316,1.7190346170293e-316,1.70893275320817e-316,1.71134853659008e-316,1.71233350586163e-316,1.7088647697753e-316,1.71128055315721e-316};
    double b[5]={3,1.47442680776014,0.963410878754794,0.717071412796616,0.567066033560999};
    double rcomp[10]={4.01201835347216,-38.1708587194998,133.028189693695,-180.33727914223,83.0639383971039,6.89811601842991e-307,1.60219306218075e-306,1.11261501989027e-306,6.95352557462179e-308,1.06764629324507e-312};

    double bcomp[5]={3,-0.184303350970018,-0.000598292354873831,-0.00500978098886239,0.00116079688704337};
    int i;
    gauss(degree + 1, w, wdim, b, r);
    bool flag=true;
    for (i=0;i<10;i++)
        if(!TestUtils::floatCompare(r[i],rcomp[i])) {
            flag=false;
            break;
        }
    QVERIFY(flag);


    flag=true;
    for (i=0;i<5;i++)
        if(!TestUtils::floatCompare(b[i],bcomp[i])) {
            flag=false;
            break;
        }
    QVERIFY(flag);

    flag=true;
    for (i=0;i<100;i++){

        if(!TestUtils::floatCompare(w[i],wcomp[i])) {
            flag=false;
            break;
        }
     }
    QVERIFY(flag);

}



void TestMzFit::testStasum() {


    double* y =  new double[6];

    y[0]=(double)2/9;
    y[1]=(double)3/9;
    y[2]=(double)4/9;
    y[3]=(double)5/9;
    y[4]=(double)6/9;
    y[5]=(double)7/9;
    double ybar=6.95293007122214e-310;
    double ysdev=2.35117935805516e-317;
    int n=6;
    stasum(y, n, &ybar, &ysdev, 1);
    QVERIFY(TestUtils::floatCompare(ybar,(double) 0.5));
    QVERIFY(TestUtils::floatCompare(ysdev,(double) 0.207869854820775));

}

void TestMzFit::testLeasqu() {
    int maxdeg=6;
    double result[10]={0};
    double w[100]={0};
    double x[6];
    double y[6];
    int ideg=2;


    x[0]=(double)5/7;
    y[0]=(double)5/9;
    x[1]=(double)8/9;
    y[1]=(double)4/9;
    x[2]=(double)6/7;
    y[2]=(double)2/9;
    x[3]=(double)3/7;
    y[3]=(double)6/9;
    x[4]=(double)1/7;
    y[4]=(double)7/9;
    x[5]=(double)2/7;
    y[5]=(double)3/9;

    leasqu(6, x, y, ideg, w, maxdeg, result);

    QVERIFY(TestUtils::floatCompare(result[0],(double) 0.701591));
    QVERIFY(TestUtils::floatCompare(result[1],(double) -0.337347));
    QVERIFY(TestUtils::floatCompare(result[2],(double) -0.0389572));
    QVERIFY(TestUtils::floatCompare(w[0],(double) 6));
    QVERIFY(TestUtils::floatCompare(w[1],(double) 3.31746));
    QVERIFY(TestUtils::floatCompare(w[2],(double) 2.32074));
    QVERIFY(TestUtils::floatCompare(w[3],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[4],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[5],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[6],(double) 3.31746));
    QVERIFY(TestUtils::floatCompare(w[7],(double) 0.486479));
    QVERIFY(TestUtils::floatCompare(w[8],(double) 0.518299));
    QVERIFY(TestUtils::floatCompare(w[9],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[10],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[11],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[12],(double) 2.32074));
    QVERIFY(TestUtils::floatCompare(w[13],(double) 0.518299));
    QVERIFY(TestUtils::floatCompare(w[14],(double) 0.0153577));
    QVERIFY(TestUtils::floatCompare(w[15],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[16],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[17],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[18],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[19],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[20],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[21],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[22],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[23],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[24],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[25],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[26],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[27],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[28],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[29],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[30],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[31],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[32],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[33],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[34],(double) 0));
    QVERIFY(TestUtils::floatCompare(w[35],(double) 0));
}

void TestMzFit::testLeasev() {
    double result[10]={4.01201835347216,-38.1708587194998,133.028189693695,-180.33727914223,83.0639383971039,6.89811601842991e-307,1.60219306218075e-306,1.11261501989027e-306,6.95352557462179e-308,1.06764629324507e-312};
    int ideg=4;
    int n=6;
    double x[6];
    x[0]=(double) 6/7;
    x[1]=(double) 2/7;
    x[2]=(double) 8/9;
    x[3]=(double) 5/7;
    x[4]=(double) 3/7;
    x[5]=(double) 1/7;
    float newrt;
    for(int ii=0; ii < n; ii++)  {
        newrt = leasev(result, ideg, x[ii]);
    }
    QVERIFY(TestUtils::floatCompare(newrt,(double)0.782730460166931));


}
