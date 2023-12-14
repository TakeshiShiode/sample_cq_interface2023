function Vu = MatGetBLDC_PWM_U()
    pwm = calllib('matlab_dll','MatlabGetBLDC_PWM_U');
    Vu = pwm;
end