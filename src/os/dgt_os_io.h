/*
 * File:   dgt_os_io.h
 * Author: dyn
 *
 * Created on November 13, 2013, 3:39 PM
 */

#ifndef DGT_OS_IO_H
#define	DGT_OS_IO_H

#ifdef	__cplusplus
extern "C" {
#endif

#define dgt_os_accept(fd,client,sz)		accept(fd,client,sz)
#define dgt_os_select(fd,r,w,x,t)		select(fd,r,w,x,t)
#define dgt_os_bind(fd,addr,len)		bind(fd,addr,len)
#define dgt_os_socket(d,t,p)			socket(d,t,p)
#define dgt_os_listen(fd,conn_max)		listen(fd,conn_max)
#define dgt_os_connect(fd,addr,sz)		connect(fd,addr,sz)

#define dgt_os_recv(fd,buf,size,flag)	recv(fd,buf,size,flag)
#define dgt_os_send(fd,buf,size,flag)	send(fd,buf,size,flag)
#define dgt_os_read(fd,buf,size)		read(fd,buf,size)
#define dgt_os_write(fd,buf,size)		write(fd,buf,size)
#define dgt_os_close(fd)				close(fd)

#define dgt_os_fopen(p,m)				fopen(p,m)
#define dgt_os_fread(b,s,n,io)			fread(b,s,n,io)
#define dgt_os_fclose(p)				fclose(p)

#define dgt_os_perror(str)				perror(str)

#ifdef	__cplusplus
}
#endif

#endif	/* DGT_OS_IO_H */

