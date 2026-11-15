SAMPLING_FREQUENCY=input('Enter sampling frequency of signal :');
TIME_INTERVAL=input('Enter time interval in form of [a b]:');
No_break_points=input('Enter number of break points:');
position=zeros(1,No_break_points+2);
for i=2:length(position)-1
    position(i)=input(['enter the ' iptnum2ordinal(i-1) ' position:']);
end
position(1,[1 end])=TIME_INTERVAL;
j=1;
signal=[];
for i=1:No_break_points+1
   TYPE_SIGNAL=input(['type of ' iptnum2ordinal(i) ' signal is:\n 1)DC signal 2)Ramp signal\n 3)polynomia signal 4)exponential signal\n 5)sinusoidal signal \n']);
   switch TYPE_SIGNAL
       %generate dc signal
       case 1
            t=linspace(position(j),position(j+1),SAMPLING_FREQUENCY*(position(j+1)-position(j)));
            amplitude=input('enter amplitude:');
            dc_signal=zeros(1,SAMPLING_FREQUENCY*(position(j+1)-position(j)))+amplitude;
            j=j+1;
            signal=[signal dc_signal];
       %generate ramp signal
       case 2
            t=linspace(position(j),position(j+1),SAMPLING_FREQUENCY*(position(j+1)-position(j)));
            slope=input('Enter slope of ramp signal:');
            intercept=input('Enter intercept of ramp signal:');
            ramp_signal= (slope.*t)+intercept;
            j=j+1;
            signal=[signal ramp_signal];
       %generate polynomial signal
       case 3
            t=linspace(position(j),position(j+1),SAMPLING_FREQUENCY*(position(j+1)-position(j)));
            power=input('enter the power:');
            intercept=input('Enter intercept of polynomial signal:');
            index=power;
            pol_signal=zeros();
            for i=1:index
            amplitude=input('enter amp:');
            pol_signal= pol_signal+(amplitude .* (t.^power));
            power=power-1;
            end
            pol_signal=pol_signal+intercept;
            j=j+1;
            signal=[signal pol_signal];
       %generate exponential 
       case 4
        t=linspace(position(j),position(j+1),SAMPLING_FREQUENCY*(position(j+1)-position(j)));
        amplitude=input('enter amp of exponential signal:');
        exponent=input('enter exponent of exponential signal:');
        exponential_signal= amplitude * exp(exponent.*t);
        j=j+1;
        signal=[signal exponential_signal];
       %generate sinusoidal signal 
       case 5 
         t=linspace(position(j),position(j+1),SAMPLING_FREQUENCY*(position(j+1)-position(j)));
         amplitude=input('enter amp of sinusoidal signal:');
         phase =input('enter the phase of signal in degree:');
         freq=input('enter the frequency of sinusoidal in Hz:');
         sinusoidal_signal=amplitude* sind(2*180*freq*t+phase);
         j=j+1;
         signal=[signal sinusoidal_signal];
   end
end
time=linspace(TIME_INTERVAL(1),TIME_INTERVAL(end),SAMPLING_FREQUENCY*(TIME_INTERVAL(end)-TIME_INTERVAL(1)));
figure;
plot(time,signal,'linewidth',2);
grid on
box on 
title({'General Signal Generator'},...
    'interpreter','tex',...
    'FontSize',20,...
    'FontName','Times')
ylabel({'signal'},...
    'interpreter','tex',...
    'FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'Time'},...
    'interpreter','tex',...
    'FontSize',20,...
    'FontName','Times','FontWeight','bold')
set(gca,'FontSize',10, 'FontName', 'Courier')
time_new=time;
NO_OPERATIONS=input('enter number of operations: ');
for i=1:NO_OPERATIONS
    TYPE_OPERATION=input(['enter type of ' iptnum2ordinal(i) ' operation\n (1)Amplitude Scaling\n (2)Time reversal\n (3)Time shift\n (4)Expanding the signal\n (5)Compressing the signal\n (6)None\n ']);
    switch TYPE_OPERATION
        % Amplitude scaling
        case 1
          time_new=time;
          amplitude = input('enter the amplitude:') ;
          signal=amplitude * signal;
        % time revesal
        case 2
            signal=fliplr(signal);
            time_new=-1 * fliplr(time_new);
        % time shift 
        case 3 
            shift_value=input('enter the shift value:');
            time_new= time - shift_value;
        %expanding signal     
        case 4
           expnading_value=input('enter the expanding value:');
           signal =resample(signal, 1, expnading_value);
           time_new=linspace(time_new(1)/expnading_value,time_new(end)/expnading_value,SAMPLING_FREQUENCY*(time_new(end)-time_new(1))/expnading_value);
         %compressing signal
         case 5
             compressing_value=input('Enter the compressing value:');
             signal=downsample(signal,compressing_value);
             time_new=linspace(time_new(1)/compressing_value,time_new(end)/compressing_value,SAMPLING_FREQUENCY*(time_new(end)-time_new(1))/compressing_value);

    end
    figure;
    plot(time_new,signal,'linewidth',2);
grid on
box on 
title({'MOdified Signal'},...
    'interpreter','tex',...
    'FontSize',20,...
    'FontName','Times')
ylabel({'signal'},...
    'interpreter','tex',...
    'FontSize',20,...
    'FontName','Times','FontWeight','bold')
xlabel({'Time'},...
    'interpreter','tex',...
    'FontSize',20,...
    'FontName','Times','FontWeight','bold')
set(gca,'FontSize',10, 'FontName','Courier')
end