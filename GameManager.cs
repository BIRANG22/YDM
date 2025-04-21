using JetBrains.Annotations;
using SupanthaPaul;
using UnityEngine;

public class GameManager : MonoBehaviour
{
    public static bool isJumpShared = true;
    public static bool isMoveShared = true;
    public static bool isDashShared = true;
    public static bool isGround = true;
    public static bool isDash = true;
    public PlayerController player1;
    public PlayerController player2;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        player2.SetGravityInverted(true);
    }

    // Update is called once per frame
    void Update()
    {
        float p1_h = 0f;
        float p2_h = 0f;
        //�����ڵ�
        if (Input.GetKeyDown(KeyCode.F) && isGround)
        {
            if (isJumpShared)
            {
                player1.Jump();
                player2.Jump();
            }
            else
            {
                player1.Jump();
            }
            isGround = false;
        }
        if (Input.GetKeyDown(KeyCode.Slash) && isGround)
        {
            if (isJumpShared)
            {
                player1.Jump();
                player2.Jump();
            }
            else
            {
                player2.Jump();
            }
            isGround = false;
        }
        //�뽬 �ڵ�
        if (Input.GetKeyDown(KeyCode.G))
        {
            if (isDashShared)
            {
                player1.Dash();
                player2.Dash();
            }
            else
            {
                player1.Dash();
            }
        }
        if (Input.GetKeyDown(KeyCode.Period))
        {
            if (isDashShared)
            {
                player1.Dash();
                player2.Dash();
            }
            else
            {
                player2.Dash();
            }
        }
        //�¿��̵� �ڵ�

        if (Input.GetKey(KeyCode.A))
        {
            if (isMoveShared)
            {
                p1_h = -1f;
                p2_h = -1f; ;
            }
            else
            {
                p1_h = -1f;
            }
        }
        if (Input.GetKey(KeyCode.D))
        {
            if (isMoveShared)
            {
                p1_h = 1f;
                p2_h = 1f; ;
            }
            else
            {
                p1_h = 1f;
            }
        }
        if (Input.GetKey(KeyCode.LeftArrow))
        {
            if (isMoveShared)
            {
                p1_h = -1f;
                p2_h = -1f; ;
            }
            else
            {
                p2_h = -1f; ;
            }
        }
        if (Input.GetKey(KeyCode.RightArrow))
        {
            if (isMoveShared)
            {
                p1_h = 1f;
                p2_h = 1f; ;
            }
            else
            {
                p2_h = 1f; ;
            }
        }
        
        player1.Move(p1_h);
        player2.Move(p2_h);

    }

    
    void OnTriggerEnter2D(Collider2D other)
    {
        if (other.CompareTag("Item"))  // �� ���� "Item"�� �װ� ���ϴ� �±׷� �ٲٸ� ��
        {
            Debug.Log("����Ű ���� ����");
            isJumpShared = false;
            // ��: ������ �Ա�, ���� ��ȭ ��
            Destroy(other.gameObject);
        }
    }

    void OnCollisionEnter2D(Collision2D collision)
    {
        if (collision.gameObject.CompareTag("Ground"))
        {
            isGround = true;
            Debug.Log("����� �浹��!");
            // ���ϴ� �ൿ ����
        }
    }
}
