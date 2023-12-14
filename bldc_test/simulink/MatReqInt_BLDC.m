function result = MatReqInt_BLDC()
    req = calllib('matlab_dll','MatlabReqInt_BLDC');
    result = req;
end