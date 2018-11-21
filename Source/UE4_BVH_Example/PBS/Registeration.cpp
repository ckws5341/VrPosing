
#include "PBS/Registeration.h"


namespace PBS
{





bool RegisterObjectToTarget(const ObjectPointSet &in_obj_point_set, const TargetPointSet &in_tar_point_set, RigidTransform &out_transf, double &out_error)
{
	const TargetPointSet &targets = in_tar_point_set;
	const ObjectPointSet &objects = in_obj_point_set;

	int num_points = targets.size();

	// Build a linear system Ax = b.
	// x is 4D vector, {cos(theta), sin(theta), translation x, translation z}.
	cml::matrixd A;
	cml::vectord x, b;
	const int num_equations = num_points*2;
	const int num_variables = 4;
	if (num_variables > num_equations) return false;
	{
		A.resize(num_equations, num_variables);
		x.resize(num_variables);
		b.resize(num_equations);

		int ii=0;
		for ( int i=0; i<num_points; i++ )
		{
			const ObjectPoint &op = objects[i];	// target points.
			const TargetPoint &tp = targets[i];	// target points.

			{
				A(ii, 0)= op[0];	
				A(ii, 1) = -1*op[2];
				A(ii, 2) = 1;
				A(ii, 3) = 0;
				b[ii] = tp[0];
				ii++;
				
				A(ii, 0)= op[2];	
				A(ii, 1)= op[0];	
				A(ii, 2)= 0;		
				A(ii, 3)= 1;		
				b[ii] = tp[2];
				ii++;
			}
			
		}
	}


	x = cml::inverse(A) * b;
	//x.solve(A, b, 0.001);

	double l = sqrt(cml::sqr(x[1]) + cml::sqr(x[0]));
	x[0] /= l;
	x[1] /= l;
	double r = -1*atan2( x[1], x[0] );	// -1 for the OpenGL coordinate.
	out_transf.r() = cml::EXP((r/2.0)*cml::y_axis_3D() );

	out_transf.t()[0] = x[2];
	out_transf.t()[1] = 0; 
	out_transf.t()[2] = x[3];


	// Linear Solving Error. 
	out_error = 0;
	if ( false )
	{

		cml::vectord e(num_equations);
		e = A * x;
		e -= b;
		for ( int i=0; i<num_equations; i++ )
		{
			out_error += cml::sqr(e[i]);
		}
	}

	// Squared Difference between targets and objects.
	out_error = 0;
	if ( true )
	{
		cml::matrixd wm(4, 4);
		wm(0, 0)=x[0];	wm(0, 1)=0;	wm(0, 2)=-x[1];	wm(0, 3)=x[2];
		wm(1, 0)=0;		wm(1, 1)=1;	wm(1, 2)=0;		wm(1, 3)=0;
		wm(2, 0)=x[1];	wm(2, 1)=0;	wm(2, 2)=x[0];	wm(2, 3)=x[3];
		wm(3, 0)=0;		wm(3, 1)=0;	wm(3, 2)=0;		wm(3, 3)=1;

		cml::matrixd pwm(4, 4);
		cml::vectord op_h(4);
		cml::vectord transformed_op(4);
		for ( int i=0; i<num_points; i++ )
		{
			const ObjectPoint &op = objects[i];	// object points.
			const TargetPoint &tp = targets[i];	// target points.

			op_h[0] = op[0];
			op_h[1] = op[1];
			op_h[2] = op[2];
			op_h[3] = 1;

			
			{
				transformed_op = wm * op_h;

				out_error += (cml::sqr((transformed_op[0]-tp[0]))
							+ cml::sqr((transformed_op[1]-tp[1]))
							+ cml::sqr((transformed_op[2]-tp[2])) );
			}
		}
	}

	

	return true;
}

bool RegisterObjectToTarget_UE4(const ObjectPointSet & in_obj_point_set, const TargetPointSet & in_tar_point_set, RigidTransform & out_transf, double & out_error)
{
	const TargetPointSet &targets = in_tar_point_set;
	const ObjectPointSet &objects = in_obj_point_set;

	int num_points = targets.size();

	// Build a linear system Ax = b.
	// x is 4D vector, {cos(theta), sin(theta), translation x, translation y}. ***UE4
	cml::matrixd A;
	cml::vectord x, b;
	const int num_equations = num_points*2;
	const int num_variables = 4;
	if (num_variables > num_equations) return false;
	{
		A.resize(num_equations, num_variables);
		x.resize(num_variables);
		b.resize(num_equations);

		int ii=0;
		for ( int i=0; i<num_points; i++ )
		{
			const ObjectPoint &op = objects[i];	// target points.
			const TargetPoint &tp = targets[i];	// target points.

			{
				A(ii, 0)= op[0];	
				A(ii, 1) = -1*op[1];
				A(ii, 2) = 1;
				A(ii, 3) = 0;
				b[ii] = tp[0];
				ii++;

				A(ii, 0)= op[1];	
				A(ii, 1)= op[0];	
				A(ii, 2)= 0;		
				A(ii, 3)= 1;		
				b[ii] = tp[1];
				ii++;
			}

		}
	}


	x = cml::inverse(A) * b;
	//x.solve(A, b, 0.001);

	double l = sqrt(cml::sqr(x[1]) + cml::sqr(x[0]));
	x[0] /= l;
	x[1] /= l;
	double r = atan2( x[1], x[0] );	// -1 for the OpenGL coordinate.
	out_transf.r() = cml::EXP((r/2.0)*cml::z_axis_3D() );

	out_transf.t()[0] = x[2];
	out_transf.t()[1] = x[3]; 
	out_transf.t()[2] = 0;


	// Linear Solving Error. 
	out_error = 0;
	if ( false )
	{

		cml::vectord e(num_equations);
		e = A * x;
		e -= b;
		for ( int i=0; i<num_equations; i++ )
		{
			out_error += cml::sqr(e[i]);
		}
	}

	// Squared Difference between targets and objects.
	out_error = 0;
	if ( true )
	{
		cml::matrixd wm(4, 4);
		wm(0, 0)=x[0];	wm(0, 1)=-x[1];	wm(0, 2)=0;	wm(0, 3)=x[2];
		wm(1, 0)=x[1];	wm(1, 1)=1;	    wm(1, 2)=0;	wm(1, 3)=x[3];
		wm(2, 0)=0;	    wm(2, 1)=x[0];	wm(2, 2)=0;	wm(2, 3)=0;
		wm(3, 0)=0;		wm(3, 1)=0;	    wm(3, 2)=0;	wm(3, 3)=1;

		cml::matrixd pwm(4, 4);
		cml::vectord op_h(4);
		cml::vectord transformed_op(4);
		for ( int i=0; i<num_points; i++ )
		{
			const ObjectPoint &op = objects[i];	// object points.
			const TargetPoint &tp = targets[i];	// target points.

			op_h[0] = op[0];
			op_h[1] = op[1];
			op_h[2] = op[2];
			op_h[3] = 1;


			{
				transformed_op = wm * op_h;

				out_error += (cml::sqr((transformed_op[0]-tp[0]))
					+ cml::sqr((transformed_op[1]-tp[1]))
					+ cml::sqr((transformed_op[2]-tp[2])) );
			}
		}
	}



	return true;
}




};