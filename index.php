<?php

header('Content-Type: text/html; charset=utf-8');

$input_query = '';
$input_local = 0;
$input_beautify = 0;
$data = null;
$error_message = null;
$raw_output = null;

if (isset($_GET['query'])) {

    $input_query = trim($_GET['query']);
    $input_local = trim($_GET['local']) == 'on' ? 1 : $input_local;
    $input_beautify = trim($_GET['beautify']) == 'on'? 1 : $input_beautify;
    if (!empty($input_query)) {

        $safe_query = escapeshellarg($input_query);
        $command = "./classfieldnum " . $safe_query;
        if($input_local === 1)
            $command .= " 'local'";
        if($input_beautify === 1)
            $command .= " 'beautify'";
        $raw_output = shell_exec($command . " 2>&1");
        if ($raw_output !== null) {
            $data = json_decode($raw_output, true);
            if (json_last_error() !== JSON_ERROR_NONE) {
                $error_message = "Ошибка: Программа вернула невалидный JSON. (см. сырой вывод ниже)";
                $data = null;
            }
        } else {
            $error_message = "Критическая ошибка: Не удалось выполнить команду.";
        }
    } else {
        $error_message = "Пожалуйста, введите строку для запроса.";
    }
}

?>
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Интерфейс для ClassFieldNum</title>
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
            margin: 20px;
            background-color: #f4f4f9;
            color: #333;
        }
        h1 {
            color: #333;
        }
        .container {
            max-width: 700px;
            margin: 0 auto;
            padding: 20px;
            background-color: #fff;
            border-radius: 8px;
            box-shadow: 0 4px 10px rgba(0,0,0,0.05);
        }
        form {
            margin-bottom: 25px;
        }
        form label {
            display: block;
            margin-bottom: 8px;
            font-weight: bold;
        }
        form input[type="text"] {
            width: 100%;
            padding: 10px;
            font-size: 16px;
            border: 1px solid #ccc;
            border-radius: 5px;
            box-sizing: border-box;
        }
        form input[type="submit"] {
            margin-top: 10px;
            padding: 10px 20px;
            font-size: 16px;
            color: #fff;
            background-color: #007bff;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.2s;
        }
        form input[type="submit"]:hover {
            background-color: #0056b3;
        }

        table {
            border-collapse: collapse;
            width: 100%;
            margin-top: 20px;
        }
        th, td {
            border: 1px solid #ddd;
            padding: 12px;
            text-align: left;
        }
        th {
            background-color: #f2f2f2;
        }
        td:first-child {
            font-weight: bold;
            color: #555;
            width: 30%;
        }

        .info {
            color: #333;
            background-color: #e6f7ff;
            border: 1px solid #b3e0ff;
            padding: 15px;
            border-radius: 5px;
            margin-top: 20px;
        }
        .error {
            color: #D8000C;
            background-color: #FFD2D2;
            border: 1px solid #D8000C;
            padding: 15px;
            border-radius: 5px;
            margin-top: 20px;
        }
        pre {
            background-color: #eee;
            border: 1px dashed #ccc;
            padding: 10px;
            white-space: pre-wrap;
            word-wrap: break-word;
        }
        hr {
            border: 0;
            height: 1px;
            background: #ddd;
            margin-top: 30px;
        }
    </style>
</head>
<body>

    <div class="container">
        <h1>Проверка ClassFieldNum</h1>

        <form action="" method="get">
            <label for="query_input">Введите строку для запроса:</label>
            <input type="text" id="query_input" name="query" value="<?php echo htmlspecialchars($input_query); ?>" placeholder="Например, +7(700)555-1234...">
            <p><input type="checkbox" id="query_beautify" name="beautify" <?php echo ($input_beautify == 1) ? 'checked="checked"' : ''; ?>> Красивое представление
            <input type="checkbox" id="query_local" name="local" <?php echo ($input_local == 1) ? 'checked="checked"' : ''; ?>> Местный код</p>
            <input type="submit" value="Отправить">
        </form>

        <hr>

        <?php if ($data): ?>

            <h2>Результаты:</h2>
            <div class="info">
                Запрос: <strong><?php echo htmlspecialchars($input_query); ?></strong>
            </div>

            <table>
                <tr>
                    <th>Параметр</th>
                    <th>Значение</th>
                </tr>
                <?php
                $assoc = array("status"=>"Статус",
                               "format"=>"Формат вывода",
                               "country"=>"Страна",
                               "dialcode"=>"Код набора",
                               "generic"=>"Общее представление",
                               "code"=>"Код",
                               "localcode"=>"Местный код");
                foreach ($data as $key => $value):
                ?>
                    <tr>
                        <td><?php echo htmlspecialchars($assoc[$key]); ?></td>
                        <td>
                            <?php
                            if (is_bool($value)) {
                                echo $value ? 'Да' : 'Нет';
                            } else {
                                if($key=='status')
                                   echo htmlspecialchars($value) . ($value ? " (Успех)" : " (Ошибка)");
                                else
                                    echo htmlspecialchars($value);
                            }
                            ?>
                        </td>
                    </tr>
                <?php endforeach; ?>
            </table>

        <?php elseif ($error_message): ?>

            <div class="error">
                <?php echo htmlspecialchars($error_message); ?>
            </div>

        <?php elseif (!isset($_GET['query'])): ?>

            <div class="info">
                Пожалуйста, введите запрос в форму выше и нажмите "Отправить".
            </div>

        <?php endif; ?>


        <?php if ($raw_output !== null): ?>
            <h3>Отладочная информация (сырой вывод программы):</h3>
            <pre><?php echo htmlspecialchars($raw_output); ?></pre>
        <?php endif; ?>
            <h3><?php echo "<a target=\"_blank\" href=\"https://github.com/badcast/ClassifieldPhoneNumber\">Github - исходный код</a>";?></h3> 
    </div>
</body>
</html>
