[signal ,fs]=wavread('sound');
length(signal)/fs %time
time=linspace(0,length(signal)/fs,length(signal));
time_1=linspace(0,1,length(signal));
%sound(signal,fs)
%display signal in time domain
figure;
subplot(3,1,1);
plot(time,signal,'linewidth',4);
grid on
box on 
title({'Signal in time domain'},'interpreter','tex','FontSize',20,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'Time'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')
set(gca,'FontSize',25, 'FontName', 'Courier')
signal_1=fftshift(fft(signal));% signal_1 is signal in frequency domain
signal_1_mag = abs(signal_1);
signal_1_phase=angle(signal_1);
fvec=linspace(-fs/2,fs/2,length(signal_1));
%display signal in  frequnecy domain
subplot(3,1,2);
plot(fvec,signal_1_mag,'linewidth',4);
grid on
box on 
title({'Magnitude in frequency domain'},'interpreter','tex','FontSize',20,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'frequancy'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')
set(gca,'FontSize',20, 'FontName', 'Courier')
subplot(3,1,3);
plot(fvec,signal_1_phase,'linewidth',4);
grid on
box on 
title({'Phase in frequency domain'},'interpreter','tex','FontSize',20,...
    'FontName','Times')
ylabel({'phase'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'frequancy'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')
set(gca,'FontSize',20, 'FontName', 'Courier')
% %sound(signal,fs);
channel=input('enter the type of the channel\n 1)delta\n 2)exp(-2*pi*5000t)\n 3)exp(-2*pi*1000t)\n 4)sinc\n 5)the fifth channel impulse\n');
switch channel
    case 1 
% %****************channel*********************
  signal_2=signal;
  time_conv=linspace(0,(length(signal)/fs)*2,(length(signal))*2-1);
  %**************channel one****************
        delta= [1 zeros(1,length(signal)-1)];
        delta=delta';
        signal_output_1(:,1)=conv(signal_2(:,1),delta);
        figure;
        plot(time_conv,signal_output_1);
grid on
box on 
title({'channel 1 impulse response'},'interpreter','tex','FontSize',30,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',30,...
    'FontName','Times','FontWeight','bold')
xlabel({'Time'},'interpreter','tex',...
    'FontSize',30,'FontName','Times','FontWeight','bold')
set(gca,'FontSize',20, 'FontName', 'Courier')    
    case 2
%**************channel two****************
        exp_1_signal=exp(-2*pi*5000*time);
        exp_1_signal=exp_1_signal';
        signal_output_2(:,1)=conv(signal_2(:,1),exp_1_signal);
        figure;
        plot(time_conv,signal_output_2);
grid on
box on 
title({'channel 2 impulse response'},'interpreter','tex','FontSize',30,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',30,...
    'FontName','Times','FontWeight','bold')
xlabel({'Time'},'interpreter','tex',...
    'FontSize',30,'FontName','Times','FontWeight','bold')
set(gca,'FontSize',20, 'FontName', 'Courier')   
    case 3
    %**************channel three****************
        exp_2_signal=exp(-2*pi*1000*time);
        exp_2_signal=exp_2_signal';
        signal_output_3(:,1)=conv(signal_2(:,1),exp_2_signal);
        figure;
        plot(time_conv,signal_output_3);
        grid on
box on 
title({'channel 3 impulse response'},'interpreter','tex','FontSize',30,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',30,...
    'FontName','Times','FontWeight','bold')
xlabel({'Time'},'interpreter','tex',...
    'FontSize',30,'FontName','Times','FontWeight','bold')
set(gca,'FontSize',20, 'FontName', 'Courier')  
    case 4
 %**************channel four****************
        sinc_signal=sinc(2*3000*time);
        sinc_signal=sinc_signal';
        signal_output_4(:,1)=conv(signal_2(:,1),sinc_signal);
        figure;
        plot(time_conv,signal_output_4);
        grid on
box on 
title({'channel 4 impulse response'},'interpreter','tex','FontSize',30,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',30,...
    'FontName','Times','FontWeight','bold')
xlabel({'Time'},'interpreter','tex',...
    'FontSize',30,'FontName','Times','FontWeight','bold')
set(gca,'FontSize',20, 'FontName', 'Courier')   
    case 5
  %**************channel five****************
        figure;
        signal_5= [2 zeros(1,1*fs-2) .5 zeros(1,length(signal)-1*fs)];
        signal_5=signal_5';
        signal_output_5(:,1)=conv(signal_2(:,1),signal_5);
        plot(time_conv,signal_output_5);
            grid on
box on 
title({'channel 5 impulse response'},'interpreter','tex','FontSize',30,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',30,...
    'FontName','Times','FontWeight','bold')
xlabel({'Time'},'interpreter','tex',...
    'FontSize',30,'FontName','Times','FontWeight','bold')
set(gca,'FontSize',20, 'FontName', 'Courier')   
end
            
            
            %noise
 sigma=input('enter the value of sigma:');
Z=sigma .*randn(1,length(time_conv));
Z=Z';
signal_output_noise_4(:,1)=signal_output_4(:,1)+Z;
signal_output_noise_3(:,1)=signal_output_3(:,1)+Z;
signal_output_noise_2(:,1)=signal_output_2(:,1)+Z;
signal_output_noise_1(:,1)=signal_output_1(:,1)+Z;
figure;
subplot(4,1,1);
plot(time,signal_output_noise_1(1:length(signal)),:);
grid on
box on 
title({'Signal on Channel 1 with noise'},'interpreter','tex','FontSize',25,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'Time'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')


subplot(4,1,2);
plot(time,signal_output_noise_2(1:length(signal)),:);
grid on
box on 
title({'Signal on Channel 2 with noise'},'interpreter','tex','FontSize',25,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'Time'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')
 

subplot(4,1,3);
plot(time,signal_output_noise_3(1:length(signal)),:);
grid on
box on 
title({'Signal on Channel 3 with noise'},'interpreter','tex','FontSize',25,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'Time'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')
 

subplot(4,1,4);
plot(time,signal_output_noise_4(1:length(signal)),:);
grid on
box on 
title({'Signal on Channel 4 with noise'},'interpreter','tex','FontSize',25,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'Time'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')
 
%******************************************************
fvec=linspace(-(2*fs-1)/2,(2*fs-1)/2,length(signal_output_noise_1));

signal_output_noise_freq_1=fftshift(fft(signal_output_noise_1));
signal_output_noise_freq_2=fftshift(fft(signal_output_noise_2));
signal_output_noise_freq_3=fftshift(fft(signal_output_noise_3));
signal_output_noise_freq_4=fftshift(fft(signal_output_noise_4));

%filter
filter_signal= zeros(1,length(signal_output_noise_freq_4));
filter_signal_1=find(fvec < -3400);
filter_signal_2=find((fvec > -3400) & (fvec < 3400));
filter_signal_3=find(fvec > 3400);
filter_signal=[zeros(1,length(filter_signal_1)) ones(1,length(filter_signal_2)) zeros(1,length(filter_signal_3))];

%**************************
figure;
subplot(4,1,1);
plot(fvec,abs(signal_output_noise_freq_1));
grid on
box on 
title({'Signal on Channel 1 with noise in freq domain'},'interpreter','tex','FontSize',25,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'frequancy'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')
 
subplot(4,1,2);
plot(fvec,abs(signal_output_noise_freq_2));
grid on
box on 
title({'Signal on Channel 2 with noise in freq domain'},'interpreter','tex','FontSize',25,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'frequancy'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')
 
subplot(4,1,3);
plot(fvec,abs(signal_output_noise_freq_3));
grid on
box on 
title({'Signal on Channel 3 with noise in freq domain'},'interpreter','tex','FontSize',25,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'frequancy'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')
 
subplot(4,1,4);
plot(fvec,abs(signal_output_noise_freq_4));
grid on
box on 
title({'Signal on Channel 4 with noise in freq domain'},'interpreter','tex','FontSize',25,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'frequancy'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')
 
%**************************
signal_filtered_1(:,1)=signal_output_noise_freq_1(:,1) .* filter_signal';
signal_filtered_2(:,1)=signal_output_noise_freq_2(:,1) .* filter_signal';
signal_filtered_3(:,1)=signal_output_noise_freq_3(:,1) .* filter_signal';
signal_filtered_4(:,1)=signal_output_noise_freq_4(:,1) .* filter_signal';

figure;
subplot(2,2,1);
plot(fvec,abs(signal_filtered_1));
grid on
box on 
title({'Filtered signal from channel 1 in frequancy domain'},'interpreter','tex','FontSize',20,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'frequancy'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')

%
subplot(2,2,2);
plot(fvec,abs(signal_filtered_1));
grid on
box on 
title({'Filtered signal from channel 2 in frequancy domain'},'interpreter','tex','FontSize',20,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'frequancy'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')

%
subplot(2,2,3);
plot(fvec,abs(signal_filtered_1));
grid on
box on 
title({'Filtered signal from channel 3 in frequancy domain'},'interpreter','tex','FontSize',20,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'frequancy'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')
 
%
subplot(2,2,4);
plot(fvec,abs(signal_filtered_1));
grid on
box on 
title({'Filtered signal from channel 4 in frequancy domain'},'interpreter','tex','FontSize',20,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'frequancy'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')

%

signal_filtered_1_time=real(ifft(ifftshift(signal_filtered_1)));
signal_filtered_2_time=real(ifft(ifftshift(signal_filtered_2)));
signal_filtered_3_time=real(ifft(ifftshift(signal_filtered_3)));
signal_filtered_4_time=real(ifft(ifftshift(signal_filtered_4)));


figure;
subplot(2,2,1);
plot(time_conv,signal_filtered_1_time);
title({'Filtered signal from channel 1 in time domain'},'interpreter','tex','FontSize',25,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'time'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')

%
subplot(2,2,2);
plot(time_conv,signal_filtered_2_time);
title({'Filtered signal from channel 2 in time domain'},'interpreter','tex','FontSize',25,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'time'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')
 
%
subplot(2,2,3);
plot(time_conv,signal_filtered_3_time);
title({'Filtered signal from channel 3 in time domain'},'interpreter','tex','FontSize',25,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'time'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')

%
subplot(2,2,4);
plot(time_conv,signal_filtered_4_time);
title({'Filtered signal from channel 4 in time domain'},'interpreter','tex','FontSize',25,...
    'FontName','Times')
ylabel({'Amplitude'},'interpreter','tex','FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'time'},'interpreter','tex',...
    'FontSize',20,'FontName','Times','FontWeight','bold')


disp(['signal filterd after channel 1 ']);
sound(signal_filtered_1_time,fs);
pause(length(signal)/fs);
disp(['signal filterd after channel 2 ']);
sound(signal_filtered_2_time,fs);
pause(length(signal)/fs);
disp(['signal filterd after channel 3 ']);
sound(signal_filtered_3_time,fs);
pause(length(signal)/fs);
disp(['signal filterd after channel 4 ']);
sound(signal_filtered_4_time,fs);
