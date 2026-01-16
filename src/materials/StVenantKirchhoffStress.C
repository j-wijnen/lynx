#include "StVenantKirchhoffStress.h"
#include "RankFourTensor.h"
#include "RankTwoTensor.h"
#include "MooseTensorUtils.h"

registerMooseObject("LynxApp", StVenantKirchhoffStress);

InputParameters
StVenantKirchhoffStress::validParams()
{
  InputParameters params = ComputeLagrangianStressPK1::validParams();
  params.addClassDescription("This material computes the Cauchy stress for small strain elasticity.");
  return params;
}

StVenantKirchhoffStress::StVenantKirchhoffStress(
    const InputParameters & parameters
)
  : ComputeLagrangianStressPK1(parameters),

  // Consumed properties
  _elasticity_tensor(getMaterialProperty<RankFourTensor>(_base_name + "elasticity_tensor"))
{
  if (!_large_kinematics)
    mooseError("large_kinematics needs to be set to true");
}

void 
StVenantKirchhoffStress::computeQpPK1Stress()
{
  // Compute the stress
  RankTwoTensor E = 0.5 * (_F[_qp].transpose() * _F[_qp] - Identity);
  RankTwoTensor S = _elasticity_tensor[_qp] * E;

  // Push forward to PK1 stress
  _pk1_stress[_qp] = _F[_qp] * S;

  // Jacobian
  enum {i_,j_,k_,l_,m_};
  RankFourTensor dE_dF = 0.5 * (outerIdentity<k_,i_,j_,l_>(_F[_qp]) + outerIdentity<k_,j_,i_,l_>(_F[_qp]));
  _pk1_jacobian[_qp] = outerIdentity<l_,j_,i_,k_>(S) + dot(_F[_qp], _elasticity_tensor[_qp]) * dE_dF;
}