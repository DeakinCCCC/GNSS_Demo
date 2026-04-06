package com.example.gnssdemo;

public class RtkLibWrapper {
    static {
        System.loadLibrary("rtklib");
    }

    /** 获取rtklib版本 */
    public static native String getRtklibVersion();

    
    // --- 内存管理工具 ---
    
    /** 分配 C 结构体空间，返回内存地址。typeName 可选: "obs_t", "obsd_t", "nav_t", "rtcm_t", "rtk_t", "prcopt_t", "sol_t", "solopt_t", "gtime_t", "sta_t" 等 **/
    public static native long mallocStruct(String typeName);
    
    /** 释放 C 结构体空间 */
    public static native void freeStruct(long addr);
    
    /** 初始化 RTK 状态结构体 (内部涉及矩阵分配) */
    public static native void rtkInit(long rtk_addr, long opt_addr);
    
    /** 释放 RTK 状态结构体内部资源 */
    public static native void rtkFree(long rtk_addr);

    
    // --- 数据输入与转换 ---
    
    /** 读取 RINEX 文件 */
    public static native int readrnx(String file, int rcv, long obs_addr, long nav_addr, long sta_addr);

    /** 解码单字节 RTCM3 数据 */
    public static native int input_rtcm3(long rtcm_addr, byte data);

    /** 从文件流解码 RTCM3 数据 */
    public static native int input_rtcm3f(long rtcm_addr, long file_ptr_addr);

    /** 初始化 RTCM 结构体 */
    public static native void init_rtcm(long rtcm_addr);


    // --- 定位解算 ---

    /** 单点定位 (SPP) */
    public static native int pntpos(long obs_addr, int n, long nav_addr, long opt_addr, long sol_addr, long azel_addr);

    /** RTK / 后处理运动定位 */
    public static native int rtkpos(long rtk_addr, long obs_addr, int n, long nav_addr);

    /** 精密单点定位 (PPP) */
    public static native void pppos(long rtk_addr, long obs_addr, int n, long nav_addr);


    // --- 坐标与时间转换 (使用 Java 数组方便读写) ---

    /** ECEF 坐标转地理坐标 (LLA) */
    public static native void ecef2pos(double[] r, double[] pos);

    /** 地理坐标 (LLA) 转 ECEF 坐标 */
    public static native void pos2ecef(double[] pos, double[] r);

    /** ECEF 增量转站心坐标 (ENU) */
    public static native void xyz2enu(double[] pos, double[] E);

    /** 站心坐标 (ENU) 转 ECEF 坐标 */
    public static native void enu2xyz(double[] pos, double[] E, double[] r);

    /** GPS 时间转 UTC 时间 */
    public static native void gpst2utc(long t_addr, long t_utc_addr);

    /** UTC 时间转 GPS 时间 */
    public static native void utc2gpst(long t_utc_addr, long t_addr);

}
