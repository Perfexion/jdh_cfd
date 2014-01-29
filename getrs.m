function getrs(nx,ny,nprefix)
    set(0,'DefaultAxesLineStyleOrder',{'-','-.','-.', ':', '-*', '-^', '-v', '-<', '->'})
    set(0,'DefaultAxesColorOrder',[0,0,0])

    load('./ghia/Re100-two.mat');
    [xx yy] = meshgrid(linspace(0,1,nx),linspace(0,1,ny));
    [xx1 yy1] = meshgrid(linspace(0,1,nx+1),linspace(0,1,ny+1));
    [xx2 yy2] = meshgrid(linspace(0,1,nx+2),linspace(0,1,ny+2));
    u = frdr('u.dat',nx,ny);
    uraw = frdr('uraw.dat',nx,ny+1);
    us = frdr('us.dat',nx,ny+1);
    v = frdr('v.dat',nx,ny);
    vraw = frdr('vraw.dat',nx,ny);
    vs = frdr('vs.dat',nx+1,ny);
    omega = frdr('omega.dat',nx,ny);
    phi = frdr('phi.dat',nx+1,ny+1);
    strm = frdr('stream.dat',nx+2,ny+2);
    
    %% Fig 1
    [xx, yy] = meshgrid(linspace(0,1,nx),linspace(0,1,ny));
    figure(1);
    contour(xx,yy,strm(2:nx+1,2:ny+1)',strmlevels)
    axis equal
    axis([0 1 0 1])
    xlabel('x');
    ylabel('y');
    title('Stream Function Contour Re=100');
    set(gcf, 'PaperPositionMode', 'manual');
    set(gcf,'PaperUnits','inches','PaperPositionMode','manual','PaperPosition',[0 0 4 4])
    print( gcf, '-dpng', [nprefix 'Stream'])
    
    %% Fig 2
    figure(2)
    contour(xx,yy,-omega',omegalevels)
    axis equal
    axis([0 1 0 1])
    xlabel('x');
    ylabel('y');
    title('Vorticity Contour Re=100');
    set(gcf, 'PaperPositionMode', 'manual');
    set(gcf,'PaperUnits','inches','PaperPositionMode','manual','PaperPosition',[0 0 4 4])
    print( gcf, '-dpng', [nprefix 'Vort'])
    
    %% Fig 3
    figure(3)
    quiver(xx,yy,u',v')
    set(gcf, 'PaperPositionMode', 'manual');
    set(gcf,'PaperUnits','inches','PaperPositionMode','manual','PaperPosition',[0 0 4 4])
    print( gcf, '-dpng', [nprefix 'Quiv'])
    
    umax = max(u(ceil(end/2),:));
    xsol = linspace(0,1,100);
    usol = -4*umax.*(xsol.^2-xsol);
    figure(4)
    plot(xx(ceil(end/2),:),u(ceil(end/2),:), xsol, usol);
    legend('Numerical','Analytical')
    xlabel('Y');
    ylabel('U Velocity');
    title('Vertical Centerline Profile');
    grid on
    set(gcf, 'PaperPositionMode', 'manual');
    set(gcf,'PaperUnits','inches','PaperPositionMode','manual','PaperPosition',[0 0 4 4])
    print( gcf, '-dpng', [nprefix 'UVel'])
    
    figure(5)
    plot(yy(:,ceil(end/2)),v(:,ceil(end/2)))
    xlabel('Y');
    ylabel('V-Velocity');
    title('Horizontal Centerline Profile');
    grid on
    set(gcf, 'PaperPositionMode', 'manual');
    set(gcf,'PaperUnits','inches','PaperPositionMode','manual','PaperPosition',[0 0 4 4])
    print( gcf, '-dpng', [nprefix 'VVel'])
    ! mv ./*.png ~/Documents/ASU/MAE598Interfaces/HW1/


