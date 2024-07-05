import requests


def fetch_data(url):
    try:
        response = requests.get(url, stream=True)
        response.raise_for_status()

        # define buffer size
        buffer_size = 5000

        # reading data in chunks
        for chunk in response.iter_content(chunk_size=buffer_size):
            if chunk:
                print(chunk.decode("utf-8"))

    except requests.exceptions.RequestException as e:
        print(f"HTTP GET request failed: {e}")


if __name__ == "__main__":
    url = "http://192.168.1.100"
    fetch_data(url)
