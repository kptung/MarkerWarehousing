package org.iii.snsi.trackingtest;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.List;

/**
 * Created by lion on 2017/4/9.
 * fast .ini file parser.
 */

/*
// demo
public class Main {
    public static void main(String[] args) {
        FastIni ini = new FastIni();
        File currentPath = new File("");
        TestBean bean = ini.fromPath(new File(currentPath.getAbsolutePath(), "translate_config.ini").
                getAbsolutePath(), TestBean.class);
        System.out.println(bean);
    }
    public static class TestBean {
        public static final String TAG = TestBean.class.getSimpleName();
        private String baidu_app_id;
        private String baidu_app_secret;
        private String google_api_key;
        private String youdao_api_key;
        private String translate_platform;
        private String phone_platform;
        private String source_language;
        private List<String> destination_language;
        private String source_file_path;
        public String getBaidu_app_id() {
            return baidu_app_id;
        }
        public void setBaidu_app_id(String baidu_app_id) {
            this.baidu_app_id = baidu_app_id;
        }
        public String getBaidu_app_secret() {
            return baidu_app_secret;
        }
        public void setBaidu_app_secret(String baidu_app_secret) {
            this.baidu_app_secret = baidu_app_secret;
        }
        public String getGoogle_api_key() {
            return google_api_key;
        }
        public void setGoogle_api_key(String google_api_key) {
            this.google_api_key = google_api_key;
        }
        public String getYoudao_api_key() {
            return youdao_api_key;
        }
        public void setYoudao_api_key(String youdao_api_key) {
            this.youdao_api_key = youdao_api_key;
        }
        public String getTranslate_platform() {
            return translate_platform;
        }
        public void setTranslate_platform(String translate_platform) {
            this.translate_platform = translate_platform;
        }
        public String getPhone_platform() {
            return phone_platform;
        }
        public void setPhone_platform(String phone_platform) {
            this.phone_platform = phone_platform;
        }
        public String getSource_language() {
            return source_language;
        }
        public void setSource_language(String source_language) {
            this.source_language = source_language;
        }
        public String getSource_file_path() {
            return source_file_path;
        }
        public void setSource_file_path(String source_file_path) {
            this.source_file_path = source_file_path;
        }
        public List<String> getDestination_language() {
            return destination_language;
        }
        public void setDestination_language(List<String> destination_language) {
            this.destination_language = destination_language;
        }

        @Override public String toString() {
            StringBuilder languages = new StringBuilder();
            try {
                for (String lang : destination_language) {
                    languages.append(lang).append(",");
                }
                languages.deleteCharAt(languages.length() - 1);
            } catch (Exception e) {
                e.printStackTrace();
            }
            return "baidu_app_id" + " = " + baidu_app_id + "\n" +
                    "baidu_app_secret" + " = " + baidu_app_secret + "\n" +
                    "google_api_key" + " = " + google_api_key + "\n" +
                    "youdao_api_key" + " = " + youdao_api_key + "\n" +
                    "translate_platform" + " = " + translate_platform + "\n" +
                    "phone_platform" + " = " + phone_platform + "\n" +
                    "source_file_path" + " = " + source_file_path + "\n" +
                    "destination_language" + " = " + languages.toString() + "\n" +
                    "source_file_path" + " = " + source_file_path + "\n";
        }
    }
}
*/

/**
 * FastIni parser.
 */
public class FastIni {

//    public static void main(String[] args) {
//        IniDocument document = new IniDocument(new File(new File("").getAbsoluteFile(),
//                "translate_config.ini").getAbsolutePath()).parse();
//        List<String> languages = document.get("destination_language");
//        if (languages != null) {
//            for (String lang : languages) {
//                System.out.println(lang);
//            }
//        }
//    }

    /**
     * parse *.ini file from path
     *
     * @param path  *.ini file path
     * @param clazz the class model you defined to receive the ini file config key values.
     * @param <T>   the template you pass in class
     * @return the data you pass in class instance.
     */
    public <T> T fromPath(String path, Class<T> clazz) {
        return fromDocument(new IniDocument(path).parse(), clazz);
    }

    public <T> T fromDocument(IniDocument document, Class<T> clazz) {

        if (document == null || document.getLines() == null ||
                document.getLines().size() <= 0) {
            return null;
        }

        try {
            // create template instance
            T instance = (T) Class.forName(clazz.getName()).newInstance();

            // get the class field you declared.
            Field[] fields = instance.getClass().getDeclaredFields();

            // set the value to the template fields from the IniDocument model.
            for (Field field : fields) {

                // filter static fields
                if (!Modifier.isStatic(field.getModifiers())) {

                    // match the ini document lines to template fields
                    for (IniDocument.KeyValue keyValue : document.getLines()) {
                        try {
                            if (keyValue.getKey() != null &&
                                    field.getName().equals(keyValue.getKey()) &&
                                    keyValue.getValue().size() > 0) {

                                /**
                                 * set the template fields accessible even if it is a private field.
                                 */
                                field.setAccessible(true);

                                // set the value to the fields by the types
                                if (field.getType() == List.class) {
                                    field.set(instance, keyValue.getValue());
                                } else {
                                    field.set(instance, keyValue.getValue().get(0));
                                }
                            }
                        } catch (Exception e) {
                        }
                    }
                }
            }

            return instance;
        } catch (IllegalAccessException |
                InstantiationException |
                ClassNotFoundException e) {
            e.printStackTrace();
        }

        return null;
    }
}

