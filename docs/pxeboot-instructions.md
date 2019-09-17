# Instructions for running tests using PXE Boot

Note: Before following the steps below, please read:
https://github.com/intel/luv-yocto/wiki/Send--LUV-test-results-to-an-HTTP-server


1. Following a successful build, the PXE Boot Package is generated at<br/>
	<work_dir>/arm-enterprise-acs/luv/build/tmp/deploy/images/qemuarm64/PXEBOOT.tar.gz

2. ftp PXEBOOT.tar.gz to the your tftp-server root directory<br/>

3. In your tftp-server root directory, unzip and untar PXEBOOT.tar.gz<br/>

	gunzip -cf PXEBOOT.tar.gz | tar xvf -

	This will generate the following file structure<br/>
	grub-efi-bootaa64.efi<br/>
	modify_luv_params_efi.py<br/>
	EFI/BOOT/sbsa/ShellSbsa.efi<br/>

4. Run command<br/>

	sudo ./modify_luv_params_efi.py -f grub-efi-bootaa64.efi -n \<Console Log Server IP\>,64001 -u http://\<Apache Server IP\>/cgi-bin/upload.php -p \<PXE Server IP\>

Note: \<PXE Server IP\> is same as tftp server IP<br/>

4. In your DHCP configuration, set <br/>
	dhcp-boot=grub-efi-bootaa64.efi

5. Invoke PXE Boot<br/>


Note:<br/>
-The results of SBSA Tests are be dispalyed on the console<br/>
-The results of linux tests will be uploaded to  http://\<Apache Server IP\><br/>
-Currently, SCT tests are not supported in PXE boot<br/>
