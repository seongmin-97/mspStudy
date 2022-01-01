%*************************************************%
% This program is filter coef. generation program %
%        Progamed by Hwang D. W. 94. 2.14.        %
%*************************************************%
clear;
close all;
clc;
disp(' ');
disp('**********************************************');
disp('*  FIR & IIR filter coefficients generation  *');
disp('**********************************************');
disp(' ');

do = 1;

%**********************************%
%       Filter type & Spec.        %
%**********************************%
while do 
  disp(' ');
  disp('Select the type of filter ?');
  disp(' ( 1=lpf, 2=hpf, 3=bpf, 4=band stop ) ');
      
  type0 = input('Filter type = ');
  if type0<0 | type0>4,
     disp(' Input Error ! ');
     break;
  end;
  
  if type0==1,
     ftype='';
  elseif type0==2,
     ftype='high';
  elseif type0==3,
     ftype='';
  elseif type0==4,
     ftype='stop';
  end;
  
  Fs=input('Enter sampling frequency (kHz) = ');
  if type0<3,
     Fc=input('Enter  cut off frequency (kHz) = ');
     if Fs/2 <= Fc,
        disp('Cut-off frequency input error !');
        break;
     end;
     Wn=Fc*2/Fs;
  else
     Fc2=input('Enter High cut off frequency (kHz) (Fc1<Fc2<Fs/2) =');
     if Fs/2 <= Fc2,
        disp('High cut off frequency input error !');
        break;
     end;
     Fc1=input('Enter Low cut off frequency (kHz) (0<Fc1<Fs/2) =');
     if Fc1 >= Fc2,
        disp('Low cut off frequency input error !');
        break;
     end;
     Wn=[Fc1*2/Fs Fc2*2/Fs];
  end;
  
  disp('Select the type of filter ?');
  disp('( 1=FIR, 2=IIR ) ');
  type1=input('Filter type= ');
  if type1>2 | type1<0,
     disp('Input Error !');
     break;
  end;
  
  taps=input('Enter the number of taps ( HPF & BSF must be "odd") ?');
  oder=taps-1;
  if type0==2 | type0==4,
     if rem(taps,2)==0,
        disp('tap be increased by 1.');
        taps=taps+1;
        oder=taps-1;
     end;
  end;
%**********************************%
%           FIR  Filter            %
%**********************************%
  if type1==1,
     disp('Enter the type of window ?');
     disp(' ( 1=Hamming, 2=Hanning, 3=Kaiser, 4=Chebyshep, 5=Blackman )');
     wind=input('Window type = ');
     if wind>5 | wind<0,
        disp('Input Error !');
        break;
     end;
     if wind==1,
        wind=hamming(taps);
      elseif wind==2,
        wind=hanning(taps);
      elseif wind==3,
        beta=input('Enter the value of beta ?'); 
        wind=kaiser(taps,beta);
      elseif wind==4,
        ripple=input('Enter the value of ripple (dB) ?');
        wind=chebwin(taps,ripple);
      elseif wind==5,
        wind=blackman(taps);
      end;
      [Z,P]=fir1(oder,Wn,ftype,wind);
  end;
%**********************************%
%           IIR  Filter            %
%**********************************%  
  if type1==2,
     disp('Select the type of filter ?');
     disp(' ( 1=Butterworth, 2=Chebyshev, 3=Elliptic )');
     type2=input('Filter type = ');
     
     if type2>3 | type2<0,
        disp('Input Error !');
        break;
     end;
     
     if type2==1,
     %   [Z,P]=butter(oder,Wn,ftype);	%HPF
	[Z,P]=butter(oder,Wn);		%LPF
     elseif type2==2,
        r=input('Enter the passband ripple (dB) = ');
        [Z,P]=cheby1(oder,r,Wn,ftype);
     elseif type2==3,
        rp=input('Enter the passband ripple (rp<<rs) (dB) = ');
        rs=input('Enter the passband ripple (rp<<rs) (dB) = ');
        [Z,P]=ellip(oder,rp,rs,Wn,ftype);
     end;
     [h,w]=freqz(Z,P,256);
  end;
  
  disp('Ideal coefficients of your filter are ');
  Z
  if type1==2,
  P
  end;
  disp(' Hit any key ! ');
  pause;
  
  [h,w]=freqz(Z,P,256);
  mag=abs(h);
  w=Fs*w/(2*pi);
  figure;

  plot(w,mag,'r');
%  title('Frequency plot ( Not rounded )');
  xlabel('frequency');
  ylabel('level');
  grid; pause; hold off; 
  figure;

  semilogy(w,mag,'r');
%  title('Frequency plot ( Not rounded )');
  xlabel('frequency');
  ylabel('level');
  grid; pause; hold off; 
%  figure;
  
%  phase=angle(h);
%  plot(w/pi,phase,'r');
%  grid;
%  title('Group Delay Plot');
%  ylabel('level');
%  xlabel('Angle Velocity(pi)');
%  disp(' Hit any key !');
%  pause;
  
  rd=input('Do you want to get rounded filter (y or n) ?','s');

  if rd=='y',
     bt=input('Enter rounding bit =  ');
     ZR=round(Z*(2^bt));
     PR=round(P*(2^bt));
     disp('Rounded coefficients of your filter are ');
     ZR
     if type1==2,
     P
     end;
     disp(' Hit any key ! ');
     pause;
     figure;
     
     [h,w]=freqz(ZR,PR,256);
     mag_r=abs(h);
     w=Fs*w/(2*pi);
     plot(w,mag_r,'g');
     title('Frequency plot ( Rounded )');
     xlabel('frequency');
     ylabel('level');
     grid; pause; hold off;
     figure;

     semilogy(w,mag_r,'g');
     title('Frequency plot ( Rounded )');
     xlabel('frequency');
     ylabel('level');
     grid; pause; hold off;
%     figure;
  
%     phase_r=angle(h);
%     plot(w/pi,phase_r,'g');
%     grid;
%     title('Group Delay Plot ( Rounded )');
%     ylabel('level');
%     xlabel('Angle Velocity(pi)');
%     disp(' Hit any key !');
%     pause;
  end;
  
  conti=input('Do you want to get another filter (y or n) ?','s');
  if conti=='n',
     break;
  end;
end;