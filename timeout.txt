// select() -> no servidor, para bloquear enquanto espera 
// pelo udp tcp


// for server
signal(SIGPIPE, SIG_IGN)
signal(SIGCHILD, SIG_IGN)

// select()


while (n < MAX_RESEND){
    sendto()
    // ...
    int ret = recvfrom()
    if (ret < 0){
        if errno == EWOULDBLOCK || errno == EAGAIN
            // timetout -> resend
            // setsockopt -> sets timeout 

    }
}


// tejo.tecnico.ulisboa.pt
// port 58011