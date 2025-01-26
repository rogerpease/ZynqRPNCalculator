module tb();

        ZynqRPNCalculator_v1_0 # (
        ) ZynqRPNCalculator_v1_0_instance 
        (
                // Ports of Axi Slave Bus Interface S00_AXI
                .s00_axi_aclk(s00_axi_aclk),
                .s00_axi_aresetn(s00_axi_aresetn),
                .s00_axi_awaddr(s00_axi_awaddr),
                .s00_axi_awprot(s00_axi_awprot),
                .s00_axi_awvalid(s00_axi_awvalid),
                .s00_axi_awready(s00_axi_awready),
                .s00_axi_wdata(s00_axi_wdata),
                .s00_axi_wstrb(s00_axi_wstrb),
                .s00_axi_wvalid(s00_axi_wvalid),
                .s00_axi_wready(s00_axi_wready),
                .s00_axi_bresp(s00_axi_bresp),
                .s00_axi_bvalid(s00_axi_bvalid),
                .s00_axi_bready(s00_axi_bready),
                .s00_axi_araddr(s00_axi_araddr),
                .s00_axi_arprot(s00_axi_arprot),
                .s00_axi_arvalid(s00_axi_arvalid),
                .s00_axi_arready(s00_axi_arready),
                .s00_axi_rdata(s00_axi_rdata),
                .s00_axi_rresp( s00_axi_rresp),
                .s00_axi_rvalid(s00_axi_rvalid),
                .s00_axi_rready(s00_axi_rready)
        );

      // Ports of Axi Slave Bus Interface S00_AXI
                reg s00_axi_aclk;
                reg s00_axi_aresetn;
                reg [3 : 0] s00_axi_awaddr;
                reg [2 : 0] s00_axi_awprot;
                reg  s00_axi_awvalid;
                wire s00_axi_awready;
                reg [31 : 0] s00_axi_wdata;
                reg [3 : 0] s00_axi_wstrb;
                reg  s00_axi_wvalid;
                wire s00_axi_wready;
                wire [1 : 0] s00_axi_bresp;
                wire s00_axi_bvalid;
                reg s00_axi_bready;
                reg [3 : 0] s00_axi_araddr;
                reg [2 : 0] s00_axi_arprot;
                reg s00_axi_arvalid;
                wire s00_axi_arready;
                wire [31 : 0] s00_axi_rdata;
                wire [1 : 0] s00_axi_rresp;
                wire s00_axi_rvalid;
                reg s00_axi_rready;


     reg [31:0] count;
     initial begin 
        count  = 0; 
        s00_axi_aclk = 0;
        while (count < 100) begin 
           #5 s00_axi_aclk = ~s00_axi_aclk;
           count = count + 1; 
        end 
        $stop();
     end 
          
     task writeregister(input reg[3:0] a,input reg[31:0]  v);
           integer wready_went_high; 
           integer awready_went_high; 
       begin 
           $display("Writing ",a," with ",v); 
           s00_axi_awvalid = 0;
           s00_axi_awaddr[3:2] = a;
           s00_axi_awaddr[1:0] = 0;
           s00_axi_wvalid = 1;
           s00_axi_awvalid = 1;
           s00_axi_wdata = v;
           s00_axi_wstrb  = 4'b1111;
           #10;
           wready_went_high = 0;
           awready_went_high = 0;
           while ((wready_went_high == 0) || (awready_went_high == 0))
           begin
               if (s00_axi_awready == 1) begin awready_went_high = 1;  end 
               if (s00_axi_wready == 1) begin wready_went_high = 1;  end 
               $display("Waiting for awready and/or wready");
               #10;
               if (awready_went_high) s00_axi_awvalid = 0; 
               if (wready_went_high) s00_axi_wvalid = 0; 
           end
           #10;
       end 
     endtask

     task readregister(input reg[3:0] a,output reg[31:0] readdata);
       begin 
           $display("Reading ",a); 
           s00_axi_awvalid = 0;
           s00_axi_araddr[3:2] = a;
           s00_axi_araddr[1:0] = 0;
           s00_axi_wvalid = 1;
           s00_axi_arvalid = 1;
           s00_axi_rready = 1;
           #10;
           while (s00_axi_arready == 0)
           begin
               $display("Waiting for awready");
               #10;
           end
           s00_axi_awvalid = 0;
           while (s00_axi_rvalid == 0)
           begin
               $display("Waiting for wready");
               #10;
           end
           s00_axi_rready = 0;
           readdata = s00_axi_rdata;
           $display("Register ",a," returned ",readdata);
       end 
     endtask

     reg [31:0] returndata; 
     
     initial begin
           $dumpvars();
           $dumpfile("wave2.vcd"); // Specify the VCD file name
           s00_axi_rready = 0;
           s00_axi_aclk = 0;
           s00_axi_awvalid = 0;
           s00_axi_araddr  = 0;
           s00_axi_arvalid = 0;
           s00_axi_aresetn = 0;
           s00_axi_bready  = 1;
           s00_axi_wstrb = 4'b0;
           s00_axi_wvalid = 0;
           #1
           #20
           #5 s00_axi_aresetn = 1;
           #10
           writeregister(0,32'h12340000);
           writeregister(3,32'h12340003);
           #10
           readregister(3,returndata);
           if (returndata != 32'h12340003) begin $display("FAILED"); $stop(1); end
           readregister(0,returndata);
           if (returndata != 32'h12340000) begin $display("FAILED"); $stop(1); end
           readregister(1,returndata);
           if (returndata != 32'h0) begin $display("FAILED"); $stop(1); end
   
           writeregister(1,32'h1); // Reset 
           writeregister(0,32'h00000010);
           writeregister(1,32'h00000002);  // Push 
           writeregister(0,32'h00000015);
           writeregister(1,32'h00000002);  // Push 
           writeregister(1,32'h00000008);  // Add 
           writeregister(0,32'h00000021);
           writeregister(1,32'h00000002);  // Push 
           writeregister(1,32'h00000020);  // Multiply
           readregister (2,returndata);    // read stack 0 
           if (returndata != 32'h25*32'h21) begin $display("FAILED"); $stop(1); end
           #10
           $stop(); 
     end





endmodule
