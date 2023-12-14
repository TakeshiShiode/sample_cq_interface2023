function Vv = MatGetBLDC_PWM_V()
    pwm = calllib('matlab_dll','MatlabGetBLDC_PWM_V');
    Vv = pwm;
end