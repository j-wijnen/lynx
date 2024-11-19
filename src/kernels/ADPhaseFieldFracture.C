#include "ADPhaseFieldFracture.h"

registerMooseObject("LynxApp", ADPhaseFieldFracture);


InputParameters
ADPhaseFieldFracture::validParams()
{
  InputParameters params = ADKernel::validParams();

  params.addRequiredParam<Real>("length_scale", "The length scale parameter.");

  // Option to set names of consumed properties
  params.addParam<MaterialPropertyName>("crack_driving_force_name", "crack_driving_force",
                                        "The name of the crack driving force property.");

  return params;
}


ADPhaseFieldFracture::ADPhaseFieldFracture(
  const InputParameters & params
)
  : ADKernel(params),
  _length_scale(getParam<Real>("length_scale")),
  _crack_driving_force(getADMaterialProperty<Real>("crack_driving_force_name"))
{
}


ADReal
ADPhaseFieldFracture::computeQpResidual()
{


  // rhs(kdp1:kdp2,1) = rhs(kdp1:kdp2,1) - dvol * ( &
  //       Ns * (Gc/lc*phi + dg*H) &
  //       + Gc*lc * matmul(transpose(dNdx), phigrad) &
  //     )

  return _test[_i][_qp] * (_u[_qp] / _length_scale + _crack_driving_force[_qp]) ; 
    + _length_scale * _grad_test[_i][_qp] * _grad_u[_qp];

}