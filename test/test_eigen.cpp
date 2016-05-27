#include <Eigen/Core>
#include <Eigen/LU>
#include <Eigen/SVD>
#include <Eigen/Array>

// import most common Eigen types 
USING_PART_OF_NAMESPACE_EIGEN

int main(int, char *[])
{
  Matrix3f m3;
  m3 << 1, 2, 3, 4, 5, 6, 7, 8, 9;
  Matrix4f m4 = Matrix4f::Identity();
  Vector4i v4(1, 2, 3, 4);

  std::cout << "m3\n" << m3 << "\nm4:\n"
    << m4 << "\nv4:\n" << v4 << std::endl;

  MatrixXf A(3,4); A  << 1,2,0,1, 1,1,1,-1, 3,1,5,-7;
  VectorXf b(3);   b << 7,3,1;
  VectorXf x;

  std::cout << std::endl;
  std::cout << A << std::endl;
  std::cout << b << std::endl;
  std::cout << std::endl;

  A.lu().solve(b, &x);
  std::cout << std::endl;
  std::cout << x;

  std::cout << std::endl;
  std::cout << A*x;

//  MatrixXf A = MatrixXf::Random(5,5);
//   VectorXf b(5); 
  // for(int i=0; i < b.size(); i++ ) std::cout << b(i) << std::endl;
  // std::cout << b.sum();
//   VectorXf x;
//  A.svd().solve(b, &x);
//
  // std::cout << VectorXi::Random(2) << std::endl;

  //SVD<MatrixXf>svdOfA(A);
 // svdOfA.solve(b, &x);


}

