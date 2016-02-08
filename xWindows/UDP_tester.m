

macHostIP = '130.91.72.120';
winHostIP = '130.91.74.15';
udpPort = 2007;

%% Open up the UDP communication.
% Both computers have to execute their open command to go
% beyond this point.
matlabUDP('close');
matlabUDP('open',winHostIP,macHostIP,udpPort);

msg = input(' What do you want to say? ','s');
while( ~isempty(msg) )
   
    matlabUDP('send', msg);
    msg = input(' What do you want to say? ','s');
end

matlabUDP('close');