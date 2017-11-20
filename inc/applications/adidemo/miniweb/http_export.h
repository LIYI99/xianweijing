#ifndef HTTP_EXPORT_H
#define HTTP_EXPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#define METHOD_DELETE           (1 << 0)
#define METHOD_GET              (1 << 1)
#define METHOD_HEAD             (1 << 2)
#define METHOD_POST             (1 << 3)
#define METHOD_PUT              (1 << 4)
/* pathological */
#define METHOD_CONNECT          (1 << 5)
#define METHOD_OPTIONS          (1 << 6)
#define METHOD_TRACE            (1 << 7)
/* WebDAV */
#define METHOD_COPY             (1 << 8)
#define METHOD_LOCK             (1 << 9)
#define METHOD_MKCOL            (1 << 10)
#define METHOD_MOVE             (1 << 11)
#define METHOD_PROPFIND         (1 << 12)
#define METHOD_PROPPATCH        (1 << 13)
#define METHOD_SEARCH           (1 << 14)
#define METHOD_UNLOCK           (1 << 15)
#define METHOD_BIND             (1 << 16)
#define METHOD_REBIND           (1 << 17)
#define METHOD_UNBIND           (1 << 18)
#define METHOD_ACL              (1 << 19)
/* subversion */
#define METHOD_REPORT           (1 << 20)
#define METHOD_MKACTIVITY       (1 << 21)
#define METHOD_CHECKOUT         (1 << 22)
#define METHOD_MERGE            (1 << 23)
/* upnp */
#define METHOD_MSEARCH          (1 << 24)
#define METHOD_NOTIFY           (1 << 25)
#define METHOD_SUBSCRIBE        (1 << 26)
#define METHOD_UNSUBSCRIBE      (1 << 27)
/* RFC-5789 */
#define METHOD_PATCH            (1 << 28)
#define METHOD_PURGE            (1 << 29)
/* CalDAV */
#define METHOD_MKCALENDAR       (1 << 30)

/* max num */
#define METHOD_MAX_NUM          (1 << 31)
#define METHOD_ALL              (METHOD_MAX_NUM - 1)


/**
	http operation functions
*/
typedef struct __HTTP_OPS HTTP_OPS;
struct __HTTP_OPS
{
	/**
		get tag : Accept, User-Agent
		name:     tag name
		return:    value
	*/
	const char* (*get_tag)          (HTTP_OPS* ops, const char* name);

	/**
		get query string :  appid=549000912&e=2&l=M&s=3
		name:                 appid
		return:                549000912
	*/
    const char* (*get_param_string) (HTTP_OPS* ops, const char* name);
	int         (*get_param_int)    (HTTP_OPS* ops, const char* name);

	/**
		get fragment xxx :   appid=549000912&e=2&l=M&s=3#xxx
		return                :   xxx
	*/
	const char* (*get_fragment)     (HTTP_OPS* ops);

	/**
		get http request body
		return:   http request body point
		len:       length of request body
	*/
	const char* (*get_body)         (HTTP_OPS* ops, int* len);


	/**
		add tag : such as Data, Content-Length
		name:     Content-Length
		value:     100
		return:    0 success, -1 fail
	*/
	int         (*add_tag)          (HTTP_OPS* ops, const char* name, const char* value);

	/**
		set http respond body
		len:       length of buffer which want to send
		return:   point of buffer
	*/
	char*       (*set_body)         (HTTP_OPS* ops, int len);

	/**
		set http respond body
		buf:      buffer which want to send
		len:      lenght of buffer
	*/
	void        (*set_body_ex)      (HTTP_OPS* ops, const char* buf, int len);

	/**
		get the raw of http request
	*/
	const char* (*get_http_request) (HTTP_OPS* ops, int* len);

	/**
		set the raw of http respond for send to client
	*/
	void        (*set_http_respond) (HTTP_OPS* ops, const char* buf, int len);

	/**
		get the request method
	*/
	int         (*get_method)       (HTTP_OPS* ops);

	/**
		disconnect from client
	*/
	void        (*disconnect)       (HTTP_OPS* ops);

	/**
		get fd for sending not http packet
	*/
	int         (*get_connection_fd)(HTTP_OPS* ops);
};


/**
	error code for cgi call back return;
*/
#define HPE_RET_SUCCESS         0
#define HPE_RET_FAIL           -1
#define HPE_RET_OUTOF_MEMORY   -2
#define HPE_RET_UNAUTHORIZED   -3

/**
	command code for special cgi call back return;
*/
//for raw packet to send
#define HPE_RET_DISCONNECT                 1      //cgi send raw packet
#define HPE_RET_DISCONNECT1                2      //cgi use http send packet
#define HPE_RET_KEEP_ALIVE                 3      //http server respond packet
#define HPE_RET_KEEP_ALIVE1                4      //http server not respond any packet


/**
	fuction point for handle cgi
	return: 0 success, -1 fail, -2 out of memory/resource
*/
typedef int (*HANDLE_FUNC)(HTTP_OPS* ops, void* arg);



/**
	ipaddr: ipaddr for http server, any 0.0.0.0
	port:    http server port
	path:   path for static html

	return: 0 success, -1 fail
*/
int http_mini_server_init(const char* ipaddr, int port, const char* path);

/**
	start http server
*/
int http_mini_server_run();

/**
	exit http server
*/
void http_mini_server_exit();

/**
	name: cgi name
	hfunc: call back fuction
	method: GET, PUSH and so on
*/
int http_mini_add_cgi_callback(const char* name, HANDLE_FUNC hfunc, int method, void* arg);

/**
	name: cgi name
	rfunc: read call back fuction
*/
int http_mini_add_read_callback(const char* name, HANDLE_FUNC rfunc);


#ifdef __cplusplus
}
#endif

#endif //HTTP_EXPORT_H

