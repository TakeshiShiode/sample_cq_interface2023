function Vw = MatGetBLDC_PWM_W()
    pwm = calllib('matlab_dll','MatlabGetBLDC_PWM_W');
    Vw = pwm;
end