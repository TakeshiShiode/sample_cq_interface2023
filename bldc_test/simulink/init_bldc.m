R = 0.453;
Ld = 0.0009447;
Lq = 0.0009447;
qsi = 0.006198;
P = 7;
J = 0.00025;
D = 0.00075;
V = 24;

result = MatHandleInit();
MatSetBLDCData(0,0,0,0,0);
%ret_u = MatGetBLDC_PWM_U();
%ret_v = MatGetBLDC_PWM_V();
%ret_w = MatGetBLDC_PWM_W();

function  result = MatHandleInit()
    loadlibrary('matlab_dll','matlab_dll.h')
    %libfunctionsview matlab_dll
    ret = calllib('matlab_dll','MatlabHandleInit');
    result = ret;
end
