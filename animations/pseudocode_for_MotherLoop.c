int framecount = 16;

int idlePixelA(int frame) {
    int effective_frame = frame % framecount;
    return 8 - abs(effective_frame(frame) -8);
}


int idlePixelB(int frame) {
    //Named variables after the shapes I'm folding the line into with the abs
    //function
    int Vee = abs(effective_frame(frame) - 8)
    int W = abs(Vee - 4);
    return 8 - W; 
}


int idlePixelC(int frame) {
    return abs(effective_frame(frame) -8);
}

int effective_frame(frame) {
    return frame % framecount; 
}
