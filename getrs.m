function getrs(nx,ny,nprefix)
    nghost = 3;
    dx = 1/(nx-1);
    dy = 1/(ny-1);
    set(0,'DefaultAxesLineStyleOrder',{'-','-.','-.', ':', '-*', '-^', '-v', '-<', '->'})
    set(0,'DefaultAxesColorOrder',[0,0,0])

    load('./ghia/Re100-two.mat');
    [xx, yy] = meshgrid(linspace(0,1,nx),linspace(0,1,ny));
    [xx1, yy1] = meshgrid(linspace(0,1,nx+1),linspace(0,1,ny+1));
    [xx2, yy2] = meshgrid(linspace(0,1,nx+2),linspace(0,1,ny+2));
    [xxg, yyg] = meshgrid(linspace(dx/2-(nghost*dx),1-dx/2+(nghost*dx),nx+2*nghost-1),...
        linspace(dy/2-(nghost*dy),1-dy/2+(nghost*dy),ny+2*nghost-1));
    u = frdr('u.dat',nx+2*nghost-1,ny+2*nghost-1);
    uraw = frdr('uraw.dat',nx+2*nghost-1,ny+2*nghost);
    us = frdr('us.dat',nx,ny+1);
    v = frdr('v.dat',nx+2*nghost-1,ny+2*nghost-1);
    vraw = frdr('vraw.dat',nx+2*nghost+1,ny+2*nghost);
    vs = frdr('vs.dat',nx+1,ny);
    omega = frdr('omega.dat',nx,ny);
    phi = frdr('phi.dat',nx+1,ny+1);
    strm = frdr('stream.dat',nx+2,ny+2);
    G = frdr('G.dat',nx+2*nghost-1, ny+2*nghost-1);
    G1 = frdr('G1.dat',nx+2*nghost-1, ny+2*nghost-1);
    dGdt = frdr('dGdt.dat',nx+2*nghost-1, ny+2*nghost-1);
%     Markers0 = frdrint('Markers.0',nx+2*nghost-1, ny+2*nghost-1);
    Markers0 = frdrint('Markers.0',nx+2*nghost-1, ny+2*nghost-1);
%     Markers1 = frdrint('Markers.1',nx+2*nghost-1, ny+2*nghost-1);
%     Markers2 = frdrint('Markers.2',nx+2*nghost-1, ny+2*nghost-1);
%     Markers100 = frdrint('Markers.100',nx+2*nghost-1, ny+2*nghost-1);
    
%     %% Fig 1
%     [xx, yy] = meshgrid(linspace(0,1,nx),linspace(0,1,ny));
%     figure(1);
%     contour(xx,yy,strm(2:nx+1,2:ny+1)',strmlevels)
%     axis equal
%     axis([0 1 0 1])
%     xlabel('x');
%     ylabel('y');
%     title('Stream Function Contour Re=100');
%     set(gcf, 'PaperPositionMode', 'manual');
%     set(gcf,'PaperUnits','inches','PaperPositionMode','manual','PaperPosition',[0 0 4 4])
%     print( gcf, '-dpng', [nprefix 'Stream'])
%     
%     %% Fig 2
%     figure(2)
%     contour(xx,yy,-omega',omegalevels)
%     axis equal
%     axis([0 1 0 1])
%     xlabel('x');
%     ylabel('y');
%     title('Vorticity Contour Re=100');
%     set(gcf, 'PaperPositionMode', 'manual');
%     set(gcf,'PaperUnits','inches','PaperPositionMode','manual','PaperPosition',[0 0 4 4])
%     print( gcf, '-dpng', [nprefix 'Vort'])
%     
    %% Fig 3
    figure(3)
    quiver(xxg,yyg,u',v',2)
    grid on
    axis equal
    set(gcf, 'PaperPositionMode', 'manual');
    set(gcf,'PaperUnits','inches','PaperPositionMode','manual','PaperPosition',[0 0 4 4])
    print( gcf, '-dpng', [nprefix 'Quiv'])
%  
%     
%     umax = max(u(ceil(end/2),:));
%     xsol = linspace(0,1,100);
%     usol = -4*umax.*(xsol.^2-xsol);
%     %%
%     figure(4)
%     plot(xx(ceil(end/2),:),u(ceil(end/2),:), xsol, usol);
%     legend('Numerical','Analytical')
%     xlabel('Y');
%     ylabel('U Velocity');
%     title('Vertical Centerline Profile');
%     grid on
%     set(gcf, 'PaperPositionMode', 'manual');
%     set(gcf,'PaperUnits','inches','PaperPositionMode','manual','PaperPosition',[0 0 4 4])
%     print( gcf, '-dpng', [nprefix 'UVel'])
%     
%     figure(5)
%     plot(yy(:,ceil(end/2)),v(:,ceil(end/2)))
%     xlabel('Y');
%     ylabel('V-Velocity');
%     title('Horizontal Centerline Profile');
%     grid on
%     set(gcf, 'PaperPositionMode', 'manual');
%     set(gcf,'PaperUnits','inches','PaperPositionMode','manual','PaperPosition',[0 0 4 4])
%     print( gcf, '-dpng', [nprefix 'VVel'])
    
    
    figure(6)
    [c,h] = contour(xxg,yyg,G',[0 0],'k','LineWidth',1);
    %clabel(c,h)1
    hold on
    [c,h] = contour(xxg,yyg,G1',[0 0],'k','LineWidth',2);
    %clabel(c,h)
    hold off
    xlabel('X');
    ylabel('Y');
    axis equal
    grid on
%     axis([0.2 0.8 0.5 1])
    set(gcf, 'PaperPositionMode', 'manual');
    set(gcf,'PaperUnits','inches','PaperPositionMode','manual','PaperPosition',[0 0 6 4])
    print( gcf, '-dpng', [nprefix 'Cont'])
    
%     figure(7)
%     [c,h] = contour(xxg,yyg,G1',[-1:.025:.5],'k');
%     clabel(c,h,'FontSize',7,'LabelSpacing',200)
%     xlabel('X');
%     ylabel('Y');
%     set(gcf, 'PaperPositionMode', 'manual');
%     set(gcf,'PaperUnits','inches','PaperPositionMode','manual','PaperPosition',[0 0 6 4])
%     print( gcf, '-dpng', [nprefix 'DCont'])

figure(9)
surf(xxg,yyg,G')

figure(10)
% subplot 221
[c,h] = contour(xxg,yyg,Markers0',[-3:-1 1:3],'k');
clabel(c,h);
% subplot 222
% [c,h] = contour(xxg,yyg,Markers1',[-3:-1 1:3],'k');
% clabel(c,h)
% subplot 223
% [c,h] = contour(xxg,yyg,Markers2',[-3:-1 1:3],'k');
% clabel(c,h);
% subplot 224
% [c,h] = contour(xxg,yyg,Markers100',[-3:-1 1:3],'k');
% clabel(c,h);
% pcolor(xxg,yyg,Markers')
% hold on;
% contour(xxg,yyg,Markers',[0 0])
% hold off;

    
%     ! mv ./*.png ~/Documents/ASU/MAE598Interfaces/HW3/


